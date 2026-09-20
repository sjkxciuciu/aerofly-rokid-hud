# Changelog

[简体中文](CHANGELOG.md) | **English**

## 2.1.0 — Landscape dashboard

- Spread flight values on the left, attitude and landing gear in the center, and flaps/spoilers on the right.
- Place throttle and connection diagnostics along the bottom while retaining large text.
- Request landscape orientation and retain a portrait fallback when firmware supplies a portrait window.

## 2.0.1 — Larger text

- Increase primary numeric text from 32 to 40 and Chinese alert text from 17 to 21 design units.
- Make Chinese labels bold and enlarge landing gear, control-surface, and connection labels.
- Widen value cards and adjust text size for longer numbers.

## 2.0.0 — Additional instruments and receiver recovery

- Add vertical speed, overall landing gear position, and connection diagnostics.
- Add an independent PC heartbeat, unicast keepalives, broadcast-address refresh, and receiver rebinding.
- Fix the receiver race during rapid pause/resume cycles.
- Discard malformed and out-of-order packets; mark stale or missing instruments as unavailable.

## 1.x — Initial prototype

- Display airspeed, altitude, attitude, flaps, spoilers, throttle percentage, and Chinese alerts.
