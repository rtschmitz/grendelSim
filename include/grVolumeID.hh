#ifndef grVolumeID_h
#define grVolumeID_h 1

#include "G4String.hh"
#include "G4VTouchable.hh"
#include "G4VPhysicalVolume.hh"

namespace grVolumeID {
enum FixedID { Unknown = 0, TunnelAir = 1, Rock = 2 };

inline G4int TrackerBase(const G4String& name) {
  const G4bool z = name.contains("_z_phys");
  if (name.contains("gargoyle_si_layer1")) return z ? 10000 : 1000;
  if (name.contains("gargoyle_si_layer3")) return z ? 20000 : 2000;
  if (name.contains("gargoyle_si_layer2")) return z ? 30000 : 3000;
  if (name.contains("gargoyle_si_layer5")) return z ? 40000 : 4000;
  return -1;
}

inline G4int FromTouchable(const G4VTouchable* touch) {
  if (!touch) return Unknown;
  const G4int depth = touch->GetHistoryDepth();
  for (G4int d = 0; d <= depth; ++d) {
    const G4VPhysicalVolume* volume = touch->GetVolume(d);
    if (!volume) continue;
    const G4String name = volume->GetName();
    const G4int base = TrackerBase(name);
    if (base >= 0) {
      const G4int local = touch->GetCopyNumber(d);
      if (name.contains("_z_phys_") && d + 1 <= depth)
        return base + 2 * touch->GetCopyNumber(d + 1) + local;
      return local >= base ? local : base + local;
    }
    if (name.contains("gargoyle_scint_phys")) return touch->GetCopyNumber(d);
    if (name == "rockPhysic") return Rock;
  }
  return TunnelAir;
}
}  // namespace grVolumeID

#endif
