##' Run critbitr unit test suite
##'
##' @param dir the direcotry containing the RUnit test files.
##' If missing, the installed test directory is used.

##' @return An RUnit \code{\link[RUnit]{RUnitTestData}} instance.
##' If there is a test failure or error, this function will raise
##' an error.
test_cbt <- function(dir) {
  if (missing(dir)) {
    dir <- system.file("test", package="critbitr")
  }
  require("RUnit", quietly=TRUE) || stop("RUnit package not found")
  suite <- defineTestSuite(name="critbitr RUnit Tests", dirs=dir,
                           testFileRegexp=".*_test\\.R$")
  result <- runTestSuite(suite)
  printTextProtocol(result, showDetails=FALSE)
  if (any_errors(result) || any_fail(result)) {
    stop("test_cbt FAIL")
  }
}

testLoadFile = function() {
  library("critbitr")
  z = make_cbt()
  .Call(critbitr:::cbt_load_file, z@cbt, "dna1.txt")
  z
}

any_errors <- function(result) {
  any(sapply(result, function(r) r$nErr > 0))
}

any_fail <- function(result) {
  any(sapply(result, function(r) r$nFail > 0))
}

