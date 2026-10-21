# Third-party components and notices

The 0.12 package is preserved as supplied, including these notices:

| Component | Supplied notice |
| --- | --- |
| OpenXR headers and loader | `deps/licenses/OpenXR_LICENSE.txt` |
| XR Sim diagnostic runtime | `diagnostics/xr-sim/LICENSE` |
| Microsoft runtime DLLs | `licenses/Microsoft_Redist.txt` and `licenses/Microsoft_ThirdPartyNotices.txt` |
| XR Tape diagnostic layer | `diagnostics/xr-tape/NOTICE.md` |

The XR Tape notice describes the OpenXR headers used by that component.
It does not assign a license to `xrtape_layer.dll` itself. The supplied
package contains no separate XR Tape project license; do not describe
that binary as MIT- or Apache-licensed on the strength of the header notice.

No new project-wide license is assigned to the mod's original code by this
import. The third-party notices retain their own scope.
