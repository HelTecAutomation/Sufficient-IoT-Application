# hdc1080 data analysis example.

- This is a decoding example officially given by TTN.[TTN analysis example.](https://www.thethingsindustries.com/docs/integrations/payload-formatters/javascript/)
```javascript
function decodeUplink(input) {
  var data = {};
  var events = {
    1: "setup",
    2: "interval",
    3: "motion",
    4: "button"
  };
  data.event = events[input.fPort];
  data.battery = (input.bytes[0] << 8) + input.bytes[1];
  data.light = (input.bytes[2] << 8) + input.bytes[3];
  data.temperature = (((input.bytes[4] & 0x80 ? input.bytes[4] - 0x100 : input.bytes[4]) << 8) + input.bytes[5]) / 100;
  var warnings = [];
  if (data.temperature < -10) {
    warnings.push("it's cold");
  }
  return {
    data: data,
    warnings: warnings
  };
}
```

```javascript
The following is a decoding example of HDC1080 and the corresponding decoded upload code.
hdc1080-uplink-decode
function decodeUplink(input) {
  var data = {};
  data.event = input.fPort;
  data.temperature = ((input.bytes[0] << 8) + input.bytes[1])/100;
  data.humidity = ((input.bytes[2] << 8) + input.bytes[3])/100;
  var warnings = [];
  if (data.temperature < -10) {
    warnings.push("it's cold");
  }
  return {
    data: data,
    warnings: warnings
  };
}
```