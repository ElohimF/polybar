#pragma once

#include "settings.hpp"

#if not WITH_XRANDR
#error "X RandR extension is disabled..."
#endif

#include <xcb/randr.h>
#include <xpp/proto/randr.hpp>

#include "common.hpp"

POLYBAR_NS

class connection;
struct position;

namespace evt {
  using randr_notify = xpp::randr::event::notify<connection&>;
  using randr_screen_change_notify = xpp::randr::event::screen_change_notify<connection&>;
}

struct backlight_values {
  uint32_t atom{0};
  double min{0.0};
  double max{0.0};
  double val{0.0};
};

struct randr_output {
  string name;
  uint16_t w{0U};
  uint16_t h{0U};
  int16_t x{0};
  int16_t y{0};
  xcb_randr_output_t output;
  backlight_values backlight;

  bool match(const string& o, bool strict = false) const;
  bool match(const position& p) const;
};

using monitor_t = shared_ptr<randr_output>;

namespace randr_util {
  void query_extension(connection& conn);

  bool check_monitor_support();

  monitor_t make_monitor(xcb_randr_output_t randr, string name, uint16_t w, uint16_t h, int16_t x, int16_t y);
  vector<monitor_t> get_monitors(connection& conn, xcb_window_t root, bool connected_only = false, bool realloc = false);

  void get_backlight_range(connection& conn, const monitor_t& mon, backlight_values& dst);
  void get_backlight_value(connection& conn, const monitor_t& mon, backlight_values& dst);
}

POLYBAR_NS_END
