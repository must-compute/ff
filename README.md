# ff

`ff` is a TUI for finding files and/or file content quickly within a directory.

## Status

Still in early development. It currently wraps `rg` and displays a scrollable list of its search results, with file
content preview where the matches are highlighted.

## Usage

```shell
ff rg foo # preview results for 'foo' (using ripgrep as the search policy)
ff ff foo # preview results for 'foo' (using ff's search policy, which isn't implemented yet)
```

- Navigate results with `Up`/`Down`.
- Open a result in Vim with `Enter`.
- Quit with `q`.

## Dependencies

- ncurses
- gtest
- ripgrep (if using the rg policy)
- vim (for opening files. The command is hardcoded for now but might be configurable in the future)

## LICENSE

to be determined