# Contributing

An important factor to keep in mind with any contribution is that this project cannot be put under an FOSS licence, as it was leaked originally. It's suspected Ubisoft is aware of the project (through the Reburrowed project) but has thus far not taken any action, though that could always change at a moments notice.

Obviously though, we **cannot** and **will not** accept financial contributions of any kind.
So any contribution of that nature isn't acceptable.

## Styleguide

For general code style, I can only ask you take advantage of the `.clang-format` config if you can in your IDE and follow whatever existing code you see.
I've written some new code which you can find under [here](src/libraries/SDK/Sources/fs), which is probably what I would prefer in a perfect world; if you're writing something new, I'd nudge you to follow that if you can.

## New Files

New source files should begin with the following.

```c++
// File created for Jaded, the community patched Jade engine
// Purpose: <purpose>
// Author:  <name>
```

Where `<purpose>` is a description of what the source file is for and `<name>` is the name of the contributors (i.e. `Author: John Doe, Winston Smith`). Any new headers should begin with the same but can exclude the second and third lines.

## LLM / Generative AI Usage

**This project doesn't allow any AI generated code, documentation or art. Period.**

It's worth taking a look [here](https://codeberg.org/ethical-foss/open-slopware/src/branch/main/why_not_llms.md) to see some of the reasons why it's not being accepted.

If you open a merge-request or issue, it's expected to be your words and your words alone.

If you speak another language, you're allowed to use your native language and translate it with available tools so long as the core of your own words are not lost. But it would be strongly _prefered_ if you try in your native language, or in English as best you can.
