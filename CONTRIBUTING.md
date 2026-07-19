## Pull Requests

Please keep pull requests **relatively small and focused**. Smaller, self-contained PRs
are much easier for me to test, validate, and review — which means they get merged faster.
If you have a large change in mind, consider splitting it into several logical PRs.

## Tests

The `master` branch is kept free of tests to stay lightweight, all tests live on the
[tests branch](https://github.com/Azzinoth/VisualNodeSystem/tree/tests).

Before submitting, your new code should **pass the existing tests**.

If a test fails but you believe the **test itself is flawed** (it happens), do not silently
work around it. Instead, call it out in your pull request:

- which test is failing.
- why you think the test is wrong.
- what you think it should check instead.

## Use of AI

There is **no "no-go" policy on AI**, you are welcome to use it.

The one thing I ask: use AI as a targeted tool not in "vibe coding" style, and **review its output before you submit.** AI tools often introduce
unneeded or unrelated changes that balloon a PR and make it hard to review. Trim those down
so the diff contains only what the change actually needs, and you do not end up with a giant,
hard-to-review pull request.

## Code Style

The conventions are **somewhat close to Unreal Engine's**:

- **PascalCase** for types, functions, and variables (including locals and parameters).
- **Braces: Allman** the opening brace goes on its own line.
- **Booleans: `b` prefix everywhere** e.g. `bHovered`, `bNewValue`, `bIsValid`.
- **Macros, compile-time constants, and enums: `ALL_CAPS_WITH_UNDERSCORES`**.
- **Single-statement `if`/`for` bodies omit braces** put the statement on the next indented line.
- **Tabs** for indentation.

## C++ Standard

The project currently targets **C++17**. You may use any language or standard-library feature up to and including C++17.