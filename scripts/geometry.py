"""Curved-tunnel geometry helpers mirroring src/grDetectorConstruction.cc.

Coordinates (all metres):
  global   : Geant4 world (x, y, z); y vertical, tunnel in the x-z plan view.
  local    : per-chord frame. lz runs along the chord, ly = global y,
             lx is transverse (positive towards the right-wall side, ID 110).
  s        : arc length along the polygonal centerline (0 at chord 0 start).
  perim    : signed arc length along the tracker "upper path" (right wall
             mid-height -> arch -> left wall mid-height) measured on the
             nominal tunnel profile, i.e. the phi-strip coordinate.
  depth    : inward distance from the nominal tunnel profile.
"""
import numba as nb
import numpy as np

TUNNEL_LENGTH = 110.0
FLOOR_WIDTH = 2.90
HEIGHT = 3.15
ARCH_WIDTH = 2.90
ARCH_HEIGHT = 1.90
WALL_HEIGHT = HEIGHT - ARCH_HEIGHT
HALF_FLOOR = 0.5 * FLOOR_WIDTH
HALF_ARCH = 0.5 * ARCH_WIDTH
ARCH_SEGMENTS = 24

_rect_area = FLOOR_WIDTH * WALL_HEIGHT
_rect_cy = 0.5 * WALL_HEIGHT
_ell_area = 0.5 * np.pi * HALF_ARCH * ARCH_HEIGHT
_ell_cy = WALL_HEIGHT + 4.0 * ARCH_HEIGHT / (3.0 * np.pi)
CENTROID_Y = (_rect_area * _rect_cy + _ell_area * _ell_cy) / (_rect_area + _ell_area)
FLOOR_Y = -CENTROID_Y
SPRING_Y = FLOOR_Y + WALL_HEIGHT
MID_WALL_Y = FLOOR_Y + 0.5 * WALL_HEIGHT

# Tracker layering (detector construction lines ~898-960).
WALL_GAP = 0.001
SUBLAYER_T = 0.015
SUBLAYER_GAP = 0.001
TRACKER_T = 2 * SUBLAYER_T + SUBLAYER_GAP
STATION_GAPS = [0.0, 0.12, 0.24, 0.36]
# station index (0 = at the wall, 3 = innermost) -> volume ID bases
PHI_BASE = [1000, 2000, 3000, 4000]
Z_ID = [10000, 20000, 30000, 40000]


def station_phi_depth(st):
    """Inner offset (from nominal profile) of the phi sublayer of a station."""
    return WALL_GAP if st == 0 else WALL_GAP + TRACKER_T + STATION_GAPS[st]


def station_z_depth(st):
    return station_phi_depth(st) + SUBLAYER_T + SUBLAYER_GAP


def _smooth5(u):
    return 10 * u**3 - 15 * u**4 + 6 * u**5


def _dense_pixels():
    pts = []
    def add(x, z):
        if pts and (x - pts[-1][0]) ** 2 + (z - pts[-1][1]) ** 2 < 1e-20:
            return
        pts.append((x, z))
    for i in range(81):
        add(0.0, 348.0 * i / 80)
    for i in range(181):
        u = i / 180
        add(-177.0 * _smooth5(u) - 1500.0 * u**3 * (1 - u) ** 3, 348.0 + 430.0 * u)
    for i in range(61):
        add(-177.0, 778.0 + 253.0 * i / 60)
    for i in range(81):
        th = np.pi - 0.5 * np.pi * i / 80
        add(-60.0 + 117.0 * np.cos(th), 1031.0 + 117.0 * np.sin(th))
    for i in range(41):
        u = i / 40
        add(-60.0 + 122.0 * u, 1148.0 - 3.0 * _smooth5(u))
    for i in range(81):
        th = 0.5 * np.pi * (1 - i / 80)
        add(62.0 + 114.0 * np.cos(th), 1030.0 + 115.0 * np.sin(th))
    for i in range(31):
        add(176.0, 1030.0 - 94.0 * i / 30)
    return np.array(pts)


def _representative_pixels():
    pts = []
    def add(x, z):
        if pts and (x - pts[-1][0]) ** 2 + (z - pts[-1][1]) ** 2 < 1e-20:
            return
        pts.append((x, z))
    for i in range(2):
        add(0.0, 348.0 * i)
    for i in range(6):
        u = i / 5
        add(-177.0 * _smooth5(u) - 1500.0 * u**3 * (1 - u) ** 3, 348.0 + 430.0 * u)
    add(-177.0, 1031.0)
    for i in range(1, 4):
        th = np.pi - 0.5 * np.pi * i / 3
        add(-60.0 + 117.0 * np.cos(th), 1031.0 + 117.0 * np.sin(th))
    add(62.0, 1145.0)
    for i in range(1, 4):
        th = 0.5 * np.pi * (1 - i / 3)
        add(62.0 + 114.0 * np.cos(th), 1030.0 + 115.0 * np.sin(th))
    add(176.0, 936.0)
    return np.array(pts)


def _build_stations():
    dense = _dense_pixels()
    off = 0.5 * (dense.min(axis=0) + dense.max(axis=0))
    rep = _representative_pixels()
    length_px = np.sum(np.hypot(*np.diff(rep, axis=0).T))
    return (rep - off) * (TUNNEL_LENGTH / length_px)  # columns: x, z


STATIONS = _build_stations()                 # (16, 2) plan-view (x, z)
N_CHORDS = len(STATIONS) - 1
_d = np.diff(STATIONS, axis=0)
CHORD_LEN = np.hypot(_d[:, 0], _d[:, 1])
CHORD_ANGLE = np.arctan2(_d[:, 0], _d[:, 1])
CHORD_DIR = np.stack([np.sin(CHORD_ANGLE), np.cos(CHORD_ANGLE)], axis=1)   # (x, z)
# rotateY(+angle) applied to local x = (1,0,0) -> (cos a, 0, -sin a)
CHORD_XAXIS = np.stack([np.cos(CHORD_ANGLE), -np.sin(CHORD_ANGLE)], axis=1)
CHORD_S0 = np.concatenate([[0.0], np.cumsum(CHORD_LEN)])[:-1]
_sa = np.empty(len(STATIONS))
_sa[0], _sa[-1] = CHORD_ANGLE[0], CHORD_ANGLE[-1]
_sa[1:-1] = np.arctan2(np.sin(CHORD_ANGLE[:-1]) + np.sin(CHORD_ANGLE[1:]),
                       np.cos(CHORD_ANGLE[:-1]) + np.cos(CHORD_ANGLE[1:]))
STATION_NORMAL = np.stack([np.sin(_sa), np.cos(_sa)], axis=1)


@nb.njit(cache=True)
def _chord_kernel(x, z, stations, normals, xaxis):
    n = len(x)
    nch = len(stations) - 1
    out = np.empty(n, np.int16)
    for j in range(n):
        best = np.inf
        o = -1
        for i in range(nch):
            a = (x[j] - stations[i, 0]) * normals[i, 0] + (z[j] - stations[i, 1]) * normals[i, 1]
            b = (x[j] - stations[i + 1, 0]) * normals[i + 1, 0] + (z[j] - stations[i + 1, 1]) * normals[i + 1, 1]
            if a >= 0 and b < 0:
                # transverse distance from the chord line, to break ties far from the tunnel
                lx = abs((x[j] - stations[i, 0]) * xaxis[i, 0] + (z[j] - stations[i, 1]) * xaxis[i, 1])
                if lx < best:
                    best = lx
                    o = i
        if o < 0:   # beyond the open ends
            a0 = (x[j] - stations[0, 0]) * normals[0, 0] + (z[j] - stations[0, 1]) * normals[0, 1]
            o = 0 if a0 < 0 else nch - 1
        out[j] = o
    return out


def chord_of(x, z):
    """Chord index owning a global plan-view point (mitre-plane logic)."""
    x = np.ascontiguousarray(x, dtype=np.float64)
    z = np.ascontiguousarray(z, dtype=np.float64)
    return _chord_kernel(x.ravel(), z.ravel(), STATIONS, STATION_NORMAL, CHORD_XAXIS).reshape(x.shape)


def to_local(x, y, z):
    """Return (chord, s, lx, ly) for global coordinates in metres."""
    c = chord_of(x, z)
    dx = x - STATIONS[c, 0]
    dz = z - STATIONS[c, 1]
    lz = dx * CHORD_DIR[c, 0] + dz * CHORD_DIR[c, 1]
    lx = dx * CHORD_XAXIS[c, 0] + dz * CHORD_XAXIS[c, 1]
    return c, CHORD_S0[c] + lz, lx, np.asarray(y, dtype=np.float64)


def to_global(s, lx, ly):
    """Inverse of to_local using the chord containing arc length s."""
    s = np.asarray(s, dtype=np.float64)
    c = np.clip(np.searchsorted(CHORD_S0, s, side="right") - 1, 0, N_CHORDS - 1)
    lz = s - CHORD_S0[c]
    x = STATIONS[c, 0] + lz * CHORD_DIR[c, 0] + lx * CHORD_XAXIS[c, 0]
    z = STATIONS[c, 1] + lz * CHORD_DIR[c, 1] + lx * CHORD_XAXIS[c, 1]
    return x, np.asarray(ly, dtype=np.float64) + 0 * x, z


# ---------------------------------------------------------------------------
# Cross-section: the tracker "upper path" polyline (right wall mid-height ->
# arch -> left wall mid-height), and helpers to get the perimeter coordinate
# and inward depth of a local (lx, ly) point.
# ---------------------------------------------------------------------------
def _upper_path():
    p = [(HALF_ARCH, MID_WALL_Y), (HALF_ARCH, SPRING_Y)]
    for i in range(1, ARCH_SEGMENTS):
        a = np.pi * i / ARCH_SEGMENTS
        p.append((HALF_ARCH * np.cos(a), SPRING_Y + ARCH_HEIGHT * np.sin(a)))
    p += [(-HALF_ARCH, SPRING_Y), (-HALF_ARCH, MID_WALL_Y)]
    return np.array(p)


UPPER_PATH = _upper_path()
_seg = np.diff(UPPER_PATH, axis=0)
_seglen = np.hypot(_seg[:, 0], _seg[:, 1])
_segdir = _seg / _seglen[:, None]
_segnorm = np.stack([-_segdir[:, 1], _segdir[:, 0]], axis=1)  # inward
_segs0 = np.concatenate([[0.0], np.cumsum(_seglen)])[:-1]
PERIM_LEN = float(np.sum(_seglen))


# Mitre directions at the path vertices (same construction as offsetPath in
# grDetectorConstruction.cc): offsetting the path by depth d moves vertex k
# to UPPER_PATH[k] + d * _miter[k].
_miter = np.empty((len(UPPER_PATH), 2))
_miter[0] = _segnorm[0]
_miter[-1] = _segnorm[-1]
for _k in range(1, len(UPPER_PATH) - 1):
    _sum = _segnorm[_k - 1] + _segnorm[_k]
    _sum /= np.hypot(*_sum)
    _miter[_k] = _sum / (_sum @ _segnorm[_k])


@nb.njit(cache=True)
def _perim_kernel(lx, ly, path, segnorm, segdir, miter, segs0, seglen):
    n = len(lx)
    nseg = len(seglen)
    perim = np.full(n, np.nan)
    depth = np.full(n, np.nan)
    for j in range(n):
        best = np.inf
        for k in range(nseg):
            rx = lx[j] - path[k, 0]
            ry = ly[j] - path[k, 1]
            d = rx * segnorm[k, 0] + ry * segnorm[k, 1]
            a0x = path[k, 0] + d * miter[k, 0]
            a0y = path[k, 1] + d * miter[k, 1]
            a1x = path[k + 1, 0] + d * miter[k + 1, 0]
            a1y = path[k + 1, 1] + d * miter[k + 1, 1]
            num = (lx[j] - a0x) * segdir[k, 0] + (ly[j] - a0y) * segdir[k, 1]
            den = (a1x - a0x) * segdir[k, 0] + (a1y - a0y) * segdir[k, 1]
            if den <= 0:
                continue
            f = num / den
            lo = -np.inf if k == 0 else 0.0
            hi = np.inf if k == nseg - 1 else 1.0
            out = max(lo - f, 0.0) + max(f - hi, 0.0)
            # prefer facets whose mitre wedge contains the point, then the
            # smallest |depth| (wedges of distant facets also reach the point)
            score = 1e3 * out + abs(d)
            if score < best:
                best = score
                perim[j] = segs0[k] + f * seglen[k]
                depth[j] = d
    return perim, depth


def perimeter_coords(lx, ly):
    """Perimeter coordinate and inward depth of local cross-section points.

    The point is located on the mitred offset of the upper path at its own
    depth: for facet k it lies between the mitre lines through vertices k
    and k+1, and its fractional position f between them maps to
    perim = s0_k + f * L_k.  This is continuous across facet corners at any
    depth (a nearest-facet projection jumps by 2 d tan(alpha/2) at the
    bisector, ~3.7 cm at the 24 cm station).  Returns (perim, depth) with
    perim in [0, PERIM_LEN] on the path and extrapolated beyond its ends.
    """
    lx = np.ascontiguousarray(lx, dtype=np.float64)
    ly = np.ascontiguousarray(ly, dtype=np.float64)
    p, d = _perim_kernel(lx.ravel(), ly.ravel(), UPPER_PATH, _segnorm, _segdir, _miter, _segs0, _seglen)
    return p.reshape(lx.shape), d.reshape(lx.shape)


def local_from_perimeter(perim, depth):
    """Inverse of perimeter_coords: local (lx, ly) of a point at a given
    perimeter coordinate and inward depth (mitred offset of the arch path)."""
    perim = np.asarray(perim, dtype=np.float64)
    depth = np.asarray(depth, dtype=np.float64)
    k = np.clip(np.searchsorted(_segs0, perim, side="right") - 1, 0, len(_seglen) - 1)
    f = (perim - _segs0[k]) / _seglen[k]
    a0 = UPPER_PATH[k] + depth[..., None] * _miter[k]
    a1 = UPPER_PATH[k + 1] + depth[..., None] * _miter[k + 1]
    p = a0 + f[..., None] * (a1 - a0)
    return p[..., 0], p[..., 1]


def surface_directions(x, y, z):
    """Unit vectors (global frame) along the tunnel and around the arch at
    the given points: the directions measured by the z and phi sublayers."""
    x = np.asarray(x, dtype=np.float64); y = np.asarray(y, dtype=np.float64); z = np.asarray(z, dtype=np.float64)
    c, _, lx, ly = to_local(x, y, z)
    perim, _ = perimeter_coords(lx, ly)
    u_s = np.stack([CHORD_DIR[c, 0], np.zeros(len(c)), CHORD_DIR[c, 1]], axis=1)
    k = np.clip(np.searchsorted(_segs0, perim, side="right") - 1, 0, len(_seglen) - 1)
    tx, ty = _segdir[k, 0], _segdir[k, 1]
    u_p = np.stack([tx * CHORD_XAXIS[c, 0], ty, tx * CHORD_XAXIS[c, 1]], axis=1)
    return u_s, u_p


def inside_profile(lx, ly, inset):
    """True if (lx, ly) is inside the tunnel cross-section shrunk by `inset`.

    The cross-section is the horseshoe: floor at FLOOR_Y, vertical walls up
    to SPRING_Y, half-ellipse arch above.  The upper region (above mid-wall
    height) is tested with the polygonal upper path used by the tracker;
    the region below mid-wall height uses the vertical walls and floor.
    """
    lx = np.asarray(lx, dtype=np.float64)
    ly = np.asarray(ly, dtype=np.float64)
    _, depth = perimeter_coords(lx, ly)
    upper_ok = depth >= inset
    lower_ok = (np.abs(lx) <= HALF_FLOOR - inset) & (ly >= FLOOR_Y + inset)
    return np.where(ly >= MID_WALL_Y, upper_ok, lower_ok & (ly < MID_WALL_Y))


# ---------------------------------------------------------------------------
# CMS interaction point, estimated by fitting the sim centreline to the top
# view of Fig. 1 of arXiv:2609.00152 (rigid fit, rms 0.3 m; reproduces the
# quoted 24-55 m IP-tunnel distances and |eta| < 0.6).  The vertical offset
# uses "22 m above the IP" relative to the sim's y = 0 (cross-section area
# centroid, 1.38 m above the floor); that reference is uncertain at ~1.4 m.
# The beam axis is close to sim +x.
# ---------------------------------------------------------------------------
IP = np.array([-2.248, -22.0, 13.028])
BEAM_AXIS = np.array([0.9993, 0.0, -0.0387])


def tunnel_coordinates(x, y, z):
    """Continuous tunnel coordinates of global points [m].

    Returns a dict of arrays: chord (tunnel segment 0-14), s (arc length
    along the centerline, 0-110 m), lx / ly (position in the tunnel cross
    section: lx towards the right wall, ly vertical), perim (position around
    the arch along the nominal tunnel surface: 0 = right wall at mid height,
    PERIM_LEN = left wall at mid height) and depth (inward distance from the
    nominal surface).  perim / depth are meaningful above mid-wall height
    (tracker region).
    """
    chord, s, lx, ly = to_local(np.asarray(x, dtype=np.float64), np.asarray(y, dtype=np.float64),
                                np.asarray(z, dtype=np.float64))
    perim, depth = perimeter_coords(lx, ly)
    return {"chord": chord, "s": s, "lx": lx, "ly": ly, "perim": perim, "depth": depth}


# ---------------------------------------------------------------------------
# Compiled single-point helper for use inside numba kernels.
# ---------------------------------------------------------------------------
GEOM_ARRAYS = (STATIONS, STATION_NORMAL, CHORD_XAXIS, UPPER_PATH, _segnorm, _segdir, _miter, _segs0, _seglen,
               np.array([MID_WALL_Y, HALF_FLOOR, FLOOR_Y]))


@nb.njit(cache=True)
def depth_inside(x, y, z, stations, normals, xaxis, path, segnorm, segdir, miter, segs0, seglen, consts):
    """Distance of a point inside the nominal tunnel surface [m]: depth below
    the arch / upper walls above mid-wall height, else distance to the lower
    walls or floor.  Same conventions as perimeter_coords / to_local."""
    nch = len(stations) - 1
    best = np.inf
    c = -1
    for i in range(nch):
        a = (x - stations[i, 0]) * normals[i, 0] + (z - stations[i, 1]) * normals[i, 1]
        b = (x - stations[i + 1, 0]) * normals[i + 1, 0] + (z - stations[i + 1, 1]) * normals[i + 1, 1]
        if a >= 0 and b < 0:
            lx = abs((x - stations[i, 0]) * xaxis[i, 0] + (z - stations[i, 1]) * xaxis[i, 1])
            if lx < best:
                best = lx
                c = i
    if c < 0:
        a0 = (x - stations[0, 0]) * normals[0, 0] + (z - stations[0, 1]) * normals[0, 1]
        c = 0 if a0 < 0 else nch - 1
    lx = (x - stations[c, 0]) * xaxis[c, 0] + (z - stations[c, 1]) * xaxis[c, 1]
    ly = y
    if ly < consts[0]:
        return min(consts[1] - abs(lx), ly - consts[2])
    nseg = len(seglen)
    bestscore = np.inf
    depth = 0.0
    for k in range(nseg):
        rx = lx - path[k, 0]
        ry = ly - path[k, 1]
        d = rx * segnorm[k, 0] + ry * segnorm[k, 1]
        a0x = path[k, 0] + d * miter[k, 0]
        a0y = path[k, 1] + d * miter[k, 1]
        a1x = path[k + 1, 0] + d * miter[k + 1, 0]
        a1y = path[k + 1, 1] + d * miter[k + 1, 1]
        num = (lx - a0x) * segdir[k, 0] + (ly - a0y) * segdir[k, 1]
        den = (a1x - a0x) * segdir[k, 0] + (a1y - a0y) * segdir[k, 1]
        if den <= 0:
            continue
        f = num / den
        lo = -np.inf if k == 0 else 0.0
        hi = np.inf if k == nseg - 1 else 1.0
        score = 1e3 * (max(lo - f, 0.0) + max(f - hi, 0.0)) + abs(d)
        if score < bestscore:
            bestscore = score
            depth = d
    return depth
