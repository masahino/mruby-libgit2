/*
** mrb_git_index.c - Git::Index class
**
** Copyright (c) masahino 2021
**
** See Copyright Notice in LICENSE
*/

#include "mruby.h"
#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/string.h"
#include "git2.h"

static void mrb_git_index_free(mrb_state *mrb, void *ptr) {
  if (ptr != NULL) {
    git_index_free((git_index *)ptr);
  }
}

const struct mrb_data_type mrb_git_index_data_type = {
  "mrb_git_index_data", mrb_git_index_free,
};

static mrb_value
mrb_git_index_add(mrb_state *mrb, mrb_value self)
{
  git_index *index = (git_index *)DATA_PTR(self);
  mrb_value entry;
  int ret;

  mrb_get_args(mrb, "o", &entry);
  switch (mrb_type(entry)) {
    case MRB_TT_STRING:
    ret = git_index_add_bypath(index, mrb_string_cstr(mrb, entry));
    break;
    default:
    mrb_raise(mrb, E_TYPE_ERROR, "type error");
    break;
  }
  if (ret < 0) {
    mrb_raisef(mrb, E_RUNTIME_ERROR, "error %d", ret);
  }
  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_git_index_path(mrb_state *mrb, mrb_value self)
{
  git_index *index = (git_index *)DATA_PTR(self);
  const char *path;
  path = git_index_path(index);

  return mrb_str_new_cstr(mrb, path);
}

void
mrb_mruby_git_index_init(mrb_state *mrb, struct RClass *git)
{
  struct RClass *index;
  index = mrb_define_class_under(mrb, git, "Index", mrb->object_class);
  MRB_SET_INSTANCE_TT(index, MRB_TT_DATA);

  mrb_define_method(mrb, index, "add", mrb_git_index_add, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, index, "path", mrb_git_index_path, MRB_ARGS_NONE());
}
