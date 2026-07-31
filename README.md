# Editor UI Scaling
haiiii

## Scaling Details (mostly for devs)
- original anchor is respected (i.e. robtop most things anchor as {0.5, 0.5} so this mod positions everything just dealing with that, other than edit tabs since edit tab api changes that anyway)
- ui scaling event triggers after ui is scaled and triggers once on init
- hook prio for original scaling is after most mods hooks (`Priority::Late`)