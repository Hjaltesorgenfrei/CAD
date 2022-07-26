# Protocol

This document describes the protocol for communicating movement to dolly.
Moves should be sent in the body of a HTTP POST request as plain text.

The moves consists least 1 to be a valid move, ending with a new line character `'\n'`, not `'\r\n'`.

## Parts

The parts in each line need to be sent in the order presented here.
Time must always be sent.
At least one of Slide Percent, Pan Angle and Tilt Angle need to be present for anything to move, if Time is sent alone, then the dolly merely waits in place, before executing the next move.
Each part is presented by a full caps string key, followed by number value and delimiting with a space.

```MOVE_CODE
TIME 100 SLIDE 10 INTERPOLATION 2
```

### Time

- Key: TIME
- Value: Time the move should take in ms. If the value is too low or high the value will be clamped to the range: TBD-TBD.
- Result: The total time the move should be executed over, changes will be interpolated using the desired method.

### Slide Percent

- Key: SLIDE
- Value: Between 0 and `max_steps` which can be found by querying `GET /max_steps`, if `max_steps` is wrong it can be calibrated by sending `POST /calibrate_slide` which recount the number of steps. Values outside this range will be clamped to it.
- Result: The Gantry will move to the step position.

### Pan Angle

- Key: PAN
- Value: 0 - 180

### Tilt Angle

- Key: TILT
- Value 0 - 180

### Interpolation Method

- Key: INTERPOLATION
