# ASCII Aquarium v2.39 Detailed Changelog

Detailed changes since v2.20. This is the more granular version for people who like release notes with a little extra kelp in the margins.

## Release Scope

This changelog covers the CYD release line from **v2.20** through the **v2.39** release candidate. It focuses on changes added after v2.20, so it does not re-document the earlier v2.20 work such as the broader settings overhaul, screenshot/capture groundwork, clock system, lighting schedule basics, or other already-released fish tank machinery.

## Backgrounds And Atmosphere

### Auto Sky

v2.39 adds a new **Auto Sky** background option. When enabled, the aquarium uses the internal clock to slowly blend the gradient colour through four time-of-day colour slots:

- **Sunrise** defaults to orange.
- **Day** defaults to blue.
- **Sunset** defaults to orange.
- **Night** defaults to dark purple.

The initial schedule uses fixed, reasonable anchors:

- Sunrise: 6:00 AM
- Day: 9:00 AM
- Sunset: 6:00 PM
- Night: 9:00 PM

That keeps the feature simple and reliable without needing latitude, longitude, or a tiny fish-shaped almanac. Timezone alone is not enough to calculate actual sunrise and sunset length accurately, so the first release keeps this intentionally sane. Future versions could add location-aware daylight if we decide the tank needs an astronomy degree.

### Auto Sky Colour Picker

The Background settings now expose a **Pick** button when Auto Sky is enabled. That opens a dedicated dialog for choosing the colours used at Sunrise, Day, Sunset, and Night.

Each slot opens the existing colour picker, so the UI reuses the same compact swatch style rather than growing a second colour-picking beast in the basement.

### Rainbow Gradient Colour

The gradient background colour system gained a **Rainbow** mode that slowly cycles through the colour spectrum. It is deliberately slow so the tank feels alive without turning into a desktop rave unless the fish specifically unionize for one.

### Cleaner Swatch Layout

One redundant green swatch was removed from the colour picker so the grid stays compact and avoids spilling into an awkward extra row.

## Clock Display Polish

### Shared Rainbow Colour Support

The clock colour picker now supports the same rainbow colour mode used by the background and ambient RGB light. That means the ASCII clock can now slowly cycle colours too, if you want the background time display to have a little nightclub energy.

### ASCII Clock Position

The large ASCII clock was nudged downward slightly. This gives it better visual breathing room below the top gradient so it looks embedded in the tank instead of bonking its head on the surface.

## Display And Layout Controls

### Flip Display

The **Flip Display** option moved out of Background settings and into Tank settings. It is a display-orientation option, not really a background option, so this puts the button where future humans are more likely to find it without needing sonar.

This is especially useful for cases and beam-splitter cube setups where the display may need to run upside down.

## Creatures

### Rare Cthulhu Octopus

A rare dark-green Cthulhu-style octopus variant was added. It can appear instead of the normal octopus, and it scares fish away more strongly than the regular octopus because, frankly, that feels polite.

There is also feed-pellet tracking support so the elder snack lord can be triggered after enough food has been dropped. The tank now contains fish, plants, bubbles, time, weather-ish sky moods, and mild cosmic dread. As one does.

### Snail

A compact orange snail was added and tuned through several passes:

- First pass introduced animated snail spawning.
- Later passes adjusted proportions and vertical placement.
- The final compact snail hugs the bottom of the tank.
- It sits low enough to feel like it is crawling along the floor rather than hovering in suspicious defiance of snail law.

The older larger snail attempt was kept out of the active release path, because sometimes the correct amount of snail is less snail.

### Jellyfish

An animated jellyfish was added with gentle swimming motion and tentacle animation. A few variants were explored, then the release settled on the first jellyfish design because it had the best balance of readability, charm, and "yep, that is probably a jellyfish."


## Timed Events

Snail and Jellyfish were promoted into the timed-event creature system. This means they are no longer only debug-spawn toys; they can now be scheduled like the octopus and seahorse.

The timed-event UI gained a cleaner creature sub-panel structure so more critters can be added without turning the main settings page into a sardine can.
