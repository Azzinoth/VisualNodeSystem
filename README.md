# Visual Node System - Tests Branch
This branch contains gtest library and the test cases and the necessary code for testing the project. The decision to create a separate tests branch was made to keep the master branch lightweight and focused on the main codebase.

## Continuous Integration
Master branch is using this branch to properly run GitHub Actions for Continuous Integration (CI). The CI pipeline is set up to automatically run test cases whenever changes are pushed to master branch or when pull requests are created.

## Third Party Licenses
This branch uses the following third-party libraries:

1) **googletest**: This library is under the BSD-3-Clause license. The full license text can be found at [googletest's GitHub repository](https://github.com/google/googletest?tab=BSD-3-Clause-1-ov-file).
