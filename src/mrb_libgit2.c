/*
** mrb_libgit2.c - Libgit2 class
**
** Copyright (c) masahino 2021
**
** See Copyright Notice in LICENSE
*/

#include "mruby.h"
#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/variable.h"
#include "mrb_libgit2.h"
#include "git2.h"
#include "mrb_git_repository.h"
#include "mrb_git_index.h"

static mrb_value
mrb_git_libgit2_init(mrb_state *mrb, mrb_value self)
{
  int ret;
  ret = git_libgit2_init();
  mrb_mod_cv_set(mrb, mrb_module_get(mrb, "Git"), mrb_intern_cstr(mrb, "@@git_init_count"),
    mrb_fixnum_value(ret));
  fprintf(stderr, "call libgit2_init %d\n", ret);
  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_git_libgit2_shutdown(mrb_state *mrb, mrb_value self)
{
  int ret;
  ret = git_libgit2_shutdown();
  mrb_mod_cv_set(mrb, mrb_module_get(mrb, "Git"), mrb_intern_cstr(mrb, "@@git_init_count"),
    mrb_fixnum_value(ret));
  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_git_libgit2_init_count(mrb_state *mrb, mrb_value self)
{
  return mrb_mod_cv_get(mrb, mrb_module_get(mrb, "Git"), mrb_intern_cstr(mrb, "@@git_init_count"));
}

void mrb_mruby_libgit2_gem_init(mrb_state *mrb)
{
  struct RClass *git;
  git = mrb_define_module(mrb, "Git");
  MRB_SET_INSTANCE_TT(git, MRB_TT_DATA);
  mrb_mod_cv_set(mrb, git, mrb_intern_cstr(mrb, "@@git_init_count"), mrb_fixnum_value(0));
  mrb_define_module_function(mrb, git, "libgit2_init", mrb_git_libgit2_init, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, git, "libgit2_shutdown", mrb_git_libgit2_shutdown, MRB_ARGS_NONE());

  mrb_define_module_function(mrb, git, "libgit2_init_count", mrb_git_libgit2_init_count, MRB_ARGS_NONE());
  mrb_mruby_git_repository_init(mrb, git);
  mrb_mruby_git_index_init(mrb, git);
}

void mrb_mruby_libgit2_gem_final(mrb_state *mrb)
{
}

