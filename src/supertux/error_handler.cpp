//  SuperTux
//  Copyright (C) 2020 A. Semphris <semphris@protonmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "supertux/error_handler.hpp"

// apparently this is exclusive to glibc as of 2020, keep checking for
// llvm/msvc equivalent from time to time   ~ Semphris
#ifdef __GLIBCXX__
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>
#endif

bool ErrorHandler::m_handing_error = false;

void
ErrorHandler::set_handlers()
{
#ifdef __GLIBCXX__
  signal(SIGSEGV, handle_error);
#endif
}

[[ noreturn ]] void
ErrorHandler::handle_error(int sig)
{
  if (m_handing_error)
  {
    // Error happened again while handling another segfault. Abort now.
    close_program();
  }
  else
  {
    m_handing_error = true;
    // Do not use external stuff (like log_fatal) to limit the risk of causing
    // another error, which would restart the handler again
    fprintf(stderr, "\nError: signal %d:\n", sig);
    print_stack_trace();
    close_program();
  }
}

void
ErrorHandler::print_stack_trace()
{
#ifdef __GLIBCXX__
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  backtrace_symbols_fd(array, static_cast<int>(size), STDERR_FILENO);
#endif
}

[[ noreturn ]] void
ErrorHandler::close_program()
{
  exit(10);
}

/* EOF */
