##
## Libgit2 Test
##

assert("Git.libgit2_init") do
  ret = Git.libgit2_init
  assert_equal 1, ret
  Git.libgit2_shutdown
end

assert("Git.libgit2_shutdown") do
  i = Git.libgit2_init
  ret = Git.libgit2_shutdown
  assert_equal i-1, ret
end

assert("Git.libgit2_init_count") do
  assert_equal 0, Git.libgit2_init_count
  Git.libgit2_init
  assert_equal 1, Git.libgit2_init_count
  Git.libgit2_shutdown
end