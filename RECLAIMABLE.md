# Reclaimable cache in ~/Library

Survey of regenerable cache. Sorted by size descending, then by path.

Three entries have since been cleared -- see below. Everything under
"Still available" is untouched.

Still available: **23.5 GB** across 22 entries

Every entry is regenerable -- an application cache, a build intermediate, or a
downloaded installer. None of it is user data. The one entry that costs
something to lose is flagged in place.

## Already cleared

Removed on 2026-08-01, freeing **14.6 GB** (6.4 GB free -> 21 GB):

| size | path |
|---|---|
| 6.85 GB | `~/Library/Caches/com.apple.dt.Xcode` |
| 3.73 GB | `~/Library/Developer/Xcode/iOS DeviceSupport/iPhone12,1 17.5.1 (21F90)` |
| 3.72 GB | `~/Library/Developer/Xcode/iOS DeviceSupport/iPhone12,1 17.6.1 (21G93)` |

The newest symbol set, `iPhone12,1 18.7.1 (22H31)`, was kept -- that is the one
matching the currently-running iOS, so deleting it would force a re-download on
the next debug session.

## Still available

| size | path | what it is |
|---|---|---|
| 4.49 GB | `~/Library/Developer/Xcode/iOS DeviceSupport/iPhone12,1 18.7.1 (22H31)` | debug symbols for a connected iPhone. Re-downloaded automatically next time you attach that device + iOS version |
| 3.68 GB | `~/Library/Caches/Firefox` | browser cache. NOT profile data -- bookmarks and logins live in Application Support |
| 2.81 GB | `~/Library/Application Support/Code/CachedExtensionVSIXs` | downloaded .vsix installers. The extensions themselves live in ~/.vscode/extensions -- this is only the installer packages |
| 2.77 GB | `~/Library/Developer/XCPGDevices` | Xcode Playground simulator devices |
| 2.37 GB | `~/Library/Developer/Xcode/UserData/Previews` | SwiftUI preview build products |
| 2.24 GB | `~/Library/Caches/vscode-cpptools` | **re-indexes afterwards.** C/C++ IntelliSense database. Safe, but VS Code rebuilds it, and on the goldensun repo that takes a while |
| 878 MB | `~/Library/Caches/com.todesktop.230313mzl4w4u92.ShipIt` | leftover app auto-update download |
| 878 MB | `~/Library/Caches/JetBrains` | IDE index and cache |
| 836 MB | `~/Library/Caches/Google` | Chrome cache. NOT profile data |
| 634 MB | `~/Library/Caches/com.microsoft.VSCode.ShipIt` | leftover VS Code auto-update download |
| 634 MB | `~/Library/Logs` | system and application logs |
| 268 MB | `~/Library/Caches/com.epicgames.EpicGamesLauncher` | launcher cache |
| 245 MB | `~/Library/Caches/Homebrew` | downloaded bottles. Use `brew cleanup` rather than deleting by hand |
| 227 MB | `~/Library/Application Support/Code/Cache` | Electron HTTP cache |
| 208 MB | `~/Library/Developer/Xcode/DerivedData` | Xcode build intermediates; every project rebuilds them |
| 122 MB | `~/Library/Caches/pip` | pip wheel cache |
| 92 MB | `~/Library/HTTPStorages` | per-application HTTP caches |
| 84 MB | `~/Library/Application Support/Code/Service Worker` | Electron service-worker cache |
| 73 MB | `~/Library/Developer/Xcode/UserData/IB Support` | Interface Builder render cache |
| 32 MB | `~/Library/Application Support/Code/logs` | application logs |
| 22 MB | `~/Library/Application Support/Code/blob_storage` | Electron blob cache |
| 10 MB | `~/Library/Application Support/Code/CachedData` | Electron V8 code cache |

## Deliberately NOT on the list

Inspected and excluded, because they hold real data:

| path | why it stays |
|---|---|
| `~/Library/Application Support/minecraft/saves` | worlds. 1.74 GB, irreplaceable |
| `~/Library/Application Support/minecraft1/saves` | worlds. 1.19 GB, irreplaceable |
| `~/Library/Application Support/minecraft*/assets` | re-downloadable, but ~4 GB of it, and the launcher stalls until it finishes |
| `~/Library/Application Support/minecraft*/{versions,libraries,runtime}` | same -- re-downloadable, slowly |
| `~/Library/Application Support/Code/User` | your VS Code settings, keybindings, snippets and workspace state |
| `~/Library/Developer/Xcode/UserData/FontAndColorThemes` | your editor themes |
| `~/Library/Developer/Xcode/UserData/KeyBindings` | your key bindings |
| `~/Library/Application Support/Steam` | installed games |
| `~/Library/Application Support/{Google,Firefox,com.operasoftware.Opera,Vivaldi}` | browser PROFILES -- bookmarks, history, saved logins. Distinct from the browser *caches* listed above as safe |
| `~/Library/Application Support/{Dropbox,Mega Limited}` | sync client state |
| `~/Library/Application Support/{OpenEmu,Dolphin,factorio,openmw,Melvor Idle}` | emulator and game saves |
| `~/Library/Caches/colima` | the Linux VM image this project builds in |
| `~/Library/Photos` | photo library data |
| `~/Library/ScreenRecordings` | recordings |
| `~/Library/{Keychains,Preferences,Containers,Mobile Documents}` | credentials, app settings, sandboxed app data, iCloud Drive. Never touch these |

## Notes

- Quit the owning application before clearing its cache, or it will simply
  rewrite the files.
- The two Minecraft installations total 12.7 GB. If one is abandoned, retiring
  it is the largest single win available -- but check `saves/` in both first.
- Browser *cache* and browser *profile* are different directories. The caches
  listed above are safe; the profiles in Application Support are not.
- Sizes were measured at generation time and will drift.

