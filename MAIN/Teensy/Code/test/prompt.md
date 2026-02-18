### Tests

## 1 Basic Test

- Per-motor test utilizing only MotorDrivers.h and other necessary libs.
- Per-Sensor test utilizing only SensorDrivers.h and other necessary libs.

## 2 Sequencer test

- Only the commandSequencer test
- Just have to print out the decrypted command(converted command from g-like code) which will be check by a person if the command is correctly interpreted

## 3 Full test

- Test in many ways with all the scripts

### Requirements

## Conditions
- Make sure that all the test code is in pairs of 2, one for mock testing and one for non-mock real testing
- Motor testing, especially non-mock one may not be able to check the motors movement digitally, therefore it will be check visually by a person.
- Some files that may be needed for testing is in src. if needed copy over the file for use in testing.

## Folder structure
test-|-mock-|- (all the different test files)
     |
     |-real-|- (all the different test files)