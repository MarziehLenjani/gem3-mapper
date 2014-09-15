/*
 * PROJECT: GEMMapper
 * FILE: report.c
 * DATE: 06/06/2012
 * AUTHOR(S): Santiago Marco-Sola <santiagomsola@gmail.com>
 * DESCRIPTION: Error handling module
 */

#include "report.h"
#include "errors.h"

// ELD-Report function
void gem_report_void(FILE* stream) {}
void (*gem_report_function)(FILE*)=gem_report_void;

// GEM error/output streams
FILE* error_stream=NULL;
FILE* log_stream=NULL;
FILE* info_stream=NULL;
FILE* debug_stream=NULL;
bool mute_error_stream = false;
bool mute_report_stream = false;

/*
 * Getters/Setters ELD-function
 */
inline report_function_t gem_error_get_report_function() {
  return gem_report_function;
}
inline void gem_error_set_report_function(report_function_t function) {
  gem_report_function = function;
}
/*
 * Getters/Setters ELD-function/ELD-streams
 */
inline FILE* gem_error_get_stream() {
  return (error_stream!=NULL) ? error_stream : stderr;
}
inline void gem_error_set_stream(FILE* const stream) {
  error_stream = stream;
}
inline FILE* gem_log_get_stream() {
  return (log_stream!=NULL) ? log_stream : stderr;
}
inline void gem_log_set_stream(FILE* const stream) {
  log_stream = stream;
}
inline FILE* gem_info_get_stream() {
  return (info_stream!=NULL) ? info_stream : stderr;
}
inline void gem_info_set_stream(FILE* const stream) {
  info_stream = stream;
}
inline FILE* gem_debug_get_stream() {
  return (debug_stream!=NULL) ? debug_stream : stderr;
}
inline void gem_debug_set_stream(FILE* const stream) {
  debug_stream = stream;
}
/*
 * Mute/Articulate ELD-streams
 */
inline void gem_mute_error_stream() {mute_error_stream=true;}
inline void gem_mute_report_stream() {mute_report_stream=true;}
inline void gem_articulate_error_stream() {mute_error_stream=false;}
inline void gem_articulate_report_stream() {mute_report_stream=false;}
inline bool gem_is_mute_error_stream() {return mute_error_stream;}
inline bool gem_is_mute_report_stream() {return mute_report_stream;}
/*
 * Time Printed Formated functions
 */
// Text weekDays & months
char* label_weekdays[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
char* label_months[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
/*
 * Time Printed Formated functions
 */
int vtfprintf(FILE* stream,const char* format,va_list v_args) {
  int chars_printed = 0; // Base printed chars
  // Get Current Time
  time_t current_time=time(0);
  struct tm local_time;
  localtime_r(&current_time,&local_time);
  // Print Current year
  chars_printed+=fprintf(stream,"%4d/%d/%d %02d:%02d:%02d -- ",
      1900+local_time.tm_year,local_time.tm_mon+1,local_time.tm_mday,
      local_time.tm_hour,local_time.tm_min,local_time.tm_sec);
  // Print Current Time
  chars_printed+=vfprintf(stream,format,v_args);
  fflush(stream);
  //  // Print Current Day
  //  if (BETWEEN(local_time.tm_wday,0,6)) {
  //    chars_printed+=fprintf(stream,"%s:",label_weekdays[local_time.tm_wday]);
  //  } else {
  //    fatal_error(TPRINTF);
  //  }
  //  // Print Current Month
  //  if (BETWEEN(local_time.tm_mon,0,11)) {
  //    chars_printed+=fprintf(stream,"%s ",label_months[local_time.tm_wday]);
  //  } else {
  //    fatal_error(TPRINTF);
  //  }
  return chars_printed;
}
int tfprintf(FILE* stream,const char* format,...) {
  va_list v_args;
  va_start(v_args,format);
  const int chars_printed = vtfprintf(stream,format,v_args);
  va_end(v_args);
  return chars_printed;
}
int vtprintf(const char* format,va_list v_args) {
  return vtfprintf(stdout,format,v_args);
}
int tprintf(const char* format,...) {
  va_list v_args;
  va_start(v_args,format);
  const int chars_printed = vtfprintf(stdout,format,v_args);
  va_end(v_args);
  return chars_printed;
}

// Tabulated Printed Formated functions
int tab_vfprintf(FILE* stream,const char* format,va_list v_args) {
  tab_global_print(stream);
  return fprintf(stream,format,v_args);
}
int tab_fprintf(FILE* stream,const char* format,...) {
  tab_global_print(stream);
  va_list v_args;
  va_start(v_args,format);
  const int chars_printed = vfprintf(stream,format,v_args);
  va_end(v_args);
  return chars_printed;
}
int tab_vprintf(const char* format,va_list v_args) {
  tab_global_print(stdout);
  return printf(format,v_args);
}
int tab_printf(const char* format,...) {
  tab_global_print(stdout);
  va_list v_args;
  va_start(v_args,format);
  const int chars_printed = vprintf(format,v_args);
  va_end(v_args);
  return chars_printed;
}
/*
 * Tabulate Data
 */
char* tabulated_table[] = {
    /*  0 */ "",
    /*  1 */ " ",
    /*  2 */ "  ",
    /*  3 */ "   ",
    /*  4 */ "    ",
    /*  5 */ "     ",
    /*  6 */ "      ",
    /*  7 */ "       ",
    /*  8 */ "        ",
    /*  9 */ "         ",
    /* 10 */ "          ",
    /* 11 */ "           ",
    /* 12 */ "            ",
    /* 13 */ "             ",
    /* 14 */ "              ",
    /* 15 */ "               ",
    /* 16 */ "                ",
    /* 17 */ "                 ",
    /* 18 */ "                  ",
    /* 19 */ "                   ",
    /* 20 */ "                    "};
inline void fprintf_tabs(FILE* const stream,const int num_spaces) {
  if (num_spaces <= 20) {
    fprintf(stream,"%s",tabulated_table[num_spaces]);
  } else {
    int i;
    for (i=0;i<num_spaces;++i) fprintf(stream," ");
  }
}
uint64_t global_tab = 0;
inline void tab_global_print(FILE* const stream) {
  fprintf_tabs(stream,global_tab);
}
inline void tab_global_inc() {
  global_tab += 2;
}
inline void tab_global_add(const uint64_t amount) {
  global_tab += amount*2;
}
inline void tab_global_dec() {
  global_tab = (global_tab > 2) ? global_tab-2 : 0;
}
inline void tab_global_subtract(const uint64_t amount) {
  global_tab -= amount*2;
}
inline void tab_global_reset() {
  global_tab = 0;
}
/*
 * Ticker
 */
GEM_INLINE void ticker_percentage_reset(
    ticker_t* const ticker,const bool enabled,const char* const label,
    const uint64_t max,const uint64_t step,const bool timed) {
  // Set status
  ticker->enabled = enabled;
  if (!ticker->enabled) return;
  ticker->finished = false;
  // Display label
  tfprintf(gem_log_get_stream(),"[%s]\n",label);
  // Type
  ticker->ticker_type=ticker_percentage;
  // Set Limits
  ticker->max=max;
  ticker->global_ticks=0;
  ticker->local_ticks=0;
  ticker->step_ticks=(step*max)/100;
  // Set Labels
  ticker->process_begin="";
  ticker->process_end="";
  ticker->finish_begin="";
  ticker->finish_end="";
  // Set time
  ticker->timed=timed;
  if (timed) clock_gettime(CLOCK_REALTIME,&ticker->begin_timer);
}
GEM_INLINE void ticker_count_reset(
    ticker_t* const ticker,const bool enabled,const char* const label,
    const uint64_t top,const uint64_t each,const bool timed) {
  // Set status
  ticker->enabled = enabled;
  if (!ticker->enabled) return;
  ticker->finished = false;
  // Display label
  tfprintf(gem_log_get_stream(),"[%s]\n",label);
  // Type
  ticker->ticker_type=ticker_count;
  // Set Limits
  ticker->max=(top!=0)?top:UINT64_MAX;
  ticker->global_ticks=0;
  ticker->local_ticks=0;
  ticker->step_ticks=each;
  // Set Labels
  ticker->process_begin="";
  ticker->process_end="";
  ticker->finish_begin="";
  ticker->finish_end="";
  // Set time
  ticker->timed=timed;
  if (timed) clock_gettime(CLOCK_REALTIME,&ticker->begin_timer);
}
GEM_INLINE void ticker_percentage_finish(ticker_t* const ticker) {
  if (!ticker->enabled || ticker->finished) return;
  // Set finished
  ticker->finished = true;
  // Print
  if (ticker->timed) {
    clock_gettime(CLOCK_REALTIME,&ticker->end_timer);
    tfprintf(gem_log_get_stream(),"%s 100%% %s... done [%2.3f s]\n",
        ticker->finish_begin,ticker->finish_end,
        TIME_DIFF_S(ticker->begin_timer,ticker->end_timer));
  } else {
    tfprintf(gem_log_get_stream(),"%s 100%% %s... done\n",ticker->finish_begin,ticker->finish_end);
  }
}
GEM_INLINE void ticker_count_finish(ticker_t* const ticker) {
  if (!ticker->enabled || ticker->finished) return;
  // Set finished
  ticker->finished = true;
  // Print
  if (ticker->timed) {
    clock_gettime(CLOCK_REALTIME,&ticker->end_timer);
    tfprintf(gem_log_get_stream(),"%s %lu %s ...done [%2.3f s]\n",
        ticker->finish_begin,ticker->global_ticks,ticker->finish_end,
        TIME_DIFF_S(ticker->begin_timer,ticker->end_timer));
  } else {
    tfprintf(gem_log_get_stream(),"%s %lu %s... done\n",ticker->finish_begin,ticker->global_ticks,ticker->finish_end);
  }
}
GEM_INLINE void ticker_update(ticker_t* const ticker,const uint64_t n) {
  if (!ticker->enabled || ticker->finished) return;
  ticker->global_ticks += n;
  ticker->local_ticks += n;
  if (gem_expect_false(ticker->global_ticks == ticker->max)) {
    if (ticker->ticker_type == ticker_percentage) {
      ticker_percentage_finish(ticker);
    } else { // ticker->ticker_type == ticker_count
      ticker_count_finish(ticker);
    }
  } else if (ticker->local_ticks >= ticker->step_ticks) {
    ticker->local_ticks -= ticker->step_ticks;
    if (ticker->ticker_type == ticker_percentage) {
      const uint64_t percentage = (ticker->global_ticks>=ticker->max) ? 100 : PERCENTAGE(ticker->global_ticks,ticker->max);
      tfprintf(gem_log_get_stream(),"%s %3d%% %s\n",ticker->process_begin,percentage,ticker->process_end);
    } else { // ticker->ticker_type == ticker_count
      tfprintf(gem_log_get_stream(),"%s %lu %s\n",
          ticker->process_begin,(ticker->global_ticks/ticker->step_ticks)*ticker->step_ticks,ticker->process_end);
    }
  }
}
GEM_INLINE void ticker_update_mutex(ticker_t* const ticker,const uint64_t n) {
  MUTEX_BEGIN_SECTION(ticker->mutex) {
    ticker_update(ticker,n);
  } MUTEX_END_SECTION(ticker->mutex);
}
GEM_INLINE void ticker_finish(ticker_t* const ticker) {
  if (!ticker->enabled) return;
  if (!ticker->finished) {
    if (ticker->ticker_type == ticker_percentage) {
      ticker_percentage_finish(ticker);
    } else { // ticker->ticker_type == ticker_count
      ticker_count_finish(ticker);
    }
  }
}
GEM_INLINE void ticker_finish_mutex(ticker_t* const ticker) {
  MUTEX_BEGIN_SECTION(ticker->mutex) {
    ticker_finish(ticker);
  } MUTEX_END_SECTION(ticker->mutex);
}
// Adding labels
GEM_INLINE void ticker_add_process_label(ticker_t* const ticker,char* const process_begin,char* const process_end) {
  ticker->process_begin=process_begin;
  ticker->process_end=process_end;
}
GEM_INLINE void ticker_add_finish_label(ticker_t* const ticker,char* const finish_begin,char* const finish_end) {
  ticker->finish_begin=finish_begin;
  ticker->finish_end=finish_end;
}
// Enable/Disable ticker
GEM_INLINE void ticker_set_status(ticker_t* const ticker,const bool enabled) {
  ticker->enabled = enabled;
}
// Set granularity
GEM_INLINE void ticker_set_step(ticker_t* const ticker,const uint64_t step) {
  ticker->step_ticks = step;
}
// Enable mutex
GEM_INLINE void ticker_mutex_enable(ticker_t* const ticker) {
  MUTEX_INIT(ticker->mutex);
}
GEM_INLINE void ticker_mutex_cleanup(ticker_t* const ticker) {
  MUTEX_DESTROY(ticker->mutex);
}
/*
 * Print's template helpers
 */
GEM_INLINE uint64_t calculate_memory_required_v(const char *template,va_list v_args) {
  // Copy to avoid spoiling v_args
  va_list v_args_cpy;
  va_copy(v_args_cpy,v_args);
  // Calculate memory required to print the template{v_args}
  uint64_t mem_required = 0, precision=0;
  const char* centinel;
  for (centinel=template;*centinel!=EOS;++centinel,precision=0) {
    if (*centinel==FORMAT) {
      ++centinel;
      // Read modifiers
      while (IS_NUMBER(*centinel)) ++centinel;
      if (*centinel==DOT){
        ++centinel;
        if (*centinel==STAR) {
          ++centinel;
          precision = va_arg(v_args_cpy,int);
        } else {
          while (IS_NUMBER(*centinel)) ++centinel;
        }
      }
      // Check format
      switch (*centinel) {
        case 's': { // String requires fetching the argument length // FIXME: %.*s
          char* const string = va_arg(v_args_cpy,char*);
          mem_required += (precision>0) ? precision : strlen(string);
          break;
        }
        default:
          // As for the rest, we estimate the memory usage
          // Also we assume an upper bound over the possible formats (int, chars, floats, ...)
          va_arg(v_args_cpy,int);
          mem_required+=20;
          break;
      }
    } else {
      ++mem_required;
    }
  }
  return mem_required;
}
GEM_INLINE uint64_t calculate_memory_required_va(const char *template,...) {
  va_list v_args;
  va_start(v_args,template);
  return calculate_memory_required_v(template,v_args);
}
