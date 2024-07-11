This is interactive bookmarks for the bash shell with fuzzy filtering.

Add this to your .bashrc

```bash
bm() {
  bmarks
  cd $(cat ~/.bookmarks_res)
  pwd
}

```

'+' - add  bookmard
'-' - remove
'ENTER' - go to the directory
'CTR-C' - abort
