setClass("CritbitTree",
         representation=representation(
           cbt="externalptr"))

make_cbt <-
function() {
  xp <- .Call(cbt_make)
  new("CritbitTree", cbt = xp)
}

insert <-
function(tree, what) {
  .Call(cbt_insert, tree@cbt, what)
}

delete <-
function(tree, what) {
  .Call(cbt_delete, tree@cbt, What)
}

contains <-
function(tree, what) {
  .Call(cbt_contains, tree@cbt, what)
}

prefix <-
function(tree, prefix) {
  .Call(cbt_prefix, tree@cbt, prefix)
}

load_file <-
function(fname) {
  z <- make_cbt()
  if (.Call(cbt_load_file, z@cbt, fname))
    z
  else
    NULL
}

