# Tunnel centerline model

This is a geometric fit to the visible tunnel centerline in the supplied image. It is expressed in a Cartesian coordinate system with origin at the centre of the lower visible endpoint, (X) increasing to the right, and (Y) increasing upward. Coordinates are initially in image pixels.

The conversion back to source-image coordinates is

\[
x_{\rm img}=X+242,\qquad y_{\rm img}=1218-Y.
\]

Define the quintic smoothstep

\[
Q(u)=10u^3-15u^4+6u^5,\qquad 0\le u\le 1.
\]

For the long P1--P2 transition, also define the localized correction

\[
C(u)=1500u^3(1-u)^3.
\]

Both `C(u)` and its first two derivatives vanish at `u=0` and `u=1`. It therefore improves the middle of the bend without moving P1 or P2 or changing the tangent/curvature matching at either join.

The centerline is the following ordered piecewise parametric curve:

\[
\mathbf r_A(u)=
\begin{pmatrix}
0\\
348u
\end{pmatrix},
\]

\[
\mathbf r_B(u)=
\begin{pmatrix}
-177Q(u)-C(u)\\
348+430u
\end{pmatrix},
\]

\[
\mathbf r_C(u)=
\begin{pmatrix}
-177\\
778+253u
\end{pmatrix},
\]

\[
\theta_D(u)=\pi-\frac{\pi}{2}u,\qquad
\mathbf r_D(u)=
\begin{pmatrix}
-60+117\cos\theta_D(u)\\
1031+117\sin\theta_D(u)
\end{pmatrix},
\]

\[
\mathbf r_E(u)=
\begin{pmatrix}
-60+122u\\
1148-3Q(u)
\end{pmatrix},
\]

\[
\theta_F(u)=\frac{\pi}{2}(1-u),\qquad
\mathbf r_F(u)=
\begin{pmatrix}
62+114\cos\theta_F(u)\\
1030+115\sin\theta_F(u)
\end{pmatrix},
\]

\[
\mathbf r_G(u)=
\begin{pmatrix}
176\\
1030-94u
\end{pmatrix}.
\]

The pieces are traversed in the order (A\to B\to C\to D\to E\to F\to G). The join points are

| Point | (X) (px) | (Y) (px) | Source (x) (px) | Source (y) (px) |
|---|---:|---:|---:|---:|
| P0 | 0 | 0 | 242 | 1218 |
| P1 | 0 | 348 | 242 | 870 |
| P2 | -177 | 778 | 65 | 440 |
| P3 | -177 | 1031 | 65 | 187 |
| P4 | -60 | 1148 | 182 | 70 |
| P5 | 62 | 1145 | 304 | 73 |
| P6 | 176 | 1030 | 418 | 188 |
| P7 | 176 | 936 | 418 | 282 |

The visible centerline length in this coordinate system is approximately 1660.6 pixels.

## Scaling and placement in Geant4

If a known map length (L) corresponds to (N) image pixels, define

\[
\lambda=\frac{L}{N}.
\]

Then use

\[
\mathbf r_{\rm G4}(u)=
\begin{pmatrix}
\lambda X(u)\\
\lambda Y(u)\\
z_0
\end{pmatrix}
\]

up to any required global translation and rotation. Sample each piece at the desired spatial interval and orient each tunnel cross-section using the local tangent. Pieces (B) and (E) have vertical/horizontal endpoint tangents respectively, so the joins are tangent-continuous to the neighbouring pieces. The right-hand endpoint is only fitted to the visible line up to its junction with the circular structure.

## Implemented Geant4 interpretation

The implementation maps the drawing into the Geant4 **X-Z plan view**; Geant4 Y remains vertical. The complete simplified centerline is normalized to an exact chord-path length of 110 m. `tunnel_centerline_samples.csv` is retained as the high-resolution reference, but it is not loaded at runtime.

The geometry preserves every analytic join while using one chord for each straight, five chords for the long smooth transition, and three chords for each quarter-turn. This gives 15 representative chords in total. Cross-sections use the angle bisector of adjacent chord directions, remain upright, and do not roll. The tunnel is a single closed `G4TessellatedSolid`, not a Boolean union: adjacent rings share their boundary exactly, so it remains hermetic without overlapping solids. The arch uses 24 facets in cross-section.

`grDetectorConstruction.cc.Tunnel` is the corrected tunnel-only implementation. `grDetectorConstruction.cc.TunnelDetector` adds the detector, and is also the active `grDetectorConstruction.cc`. The world is air and remains visually hidden. A finite 1 m-thick swept rock shell is volume ID 2 and uses the same translucent brown-gray appearance as the straight main-branch geometry. The explicit tunnel-air volume is ID 1 and is invisible; detector shells are placed inside it. Only the defined shell is concrete, so particles outside it do not incur rock-transport cost.

## Detector sweep and performance

The full version sweeps 17 continuous scintillator shells along exactly the same stations: floor veto; left and right wall vetoes; wall extensions for the three inner stations; and two 1.5 cm tracker sublayers at each of the wall, 12 cm, 24 cm, and 36 cm stations. The sublayers have a 1 mm air gap, and station distances are measured edge-to-edge from the inner face of the wall tracker, matching the straight geometry convention.

Each complete shell is also one closed tessellated sweep following the same 15 centerline chords. The floor and wall vetoes meet on a shared boundary without overlapping. This removes the expensive Boolean-solid polygonization that previously made visualization take tens of seconds or minutes. The complete detector scene exports through the Geant4 VRML driver in about 0.5 seconds on the local test machine.

The shell copy numbers identify the continuous station and sublayer, not individual 1 cm channels. If 1 cm channel identity is required in output, it should be calculated from hit position and centerline arc length in the sensitive detector rather than represented as physical Geant4 boundaries. That preserves direct integer channel IDs without the navigation cost of physical segmentation.
