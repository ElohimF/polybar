#include <cassert>

#include "drawtypes/label.hpp"
#include "modules/github.hpp"

#include "modules/meta/base.inl"

POLYBAR_NS

namespace modules {
  template class module<github_module>;

  /**
   * Construct module
   */
  github_module::github_module(const bar_settings& bar, string name_)
      : timer_module<github_module>(bar, move(name_)), m_http(http_util::make_downloader()) {
    m_accesstoken = m_conf.get(name(), "token");
    m_interval = m_conf.get<decltype(m_interval)>(name(), "interval", 60s);
    m_empty_notifications = m_conf.get(name(), "empty-notifications", m_empty_notifications);

    m_formatter->add(DEFAULT_FORMAT, TAG_LABEL, {TAG_LABEL});

    if (m_formatter->has(TAG_LABEL)) {
      m_label = load_optional_label(m_conf, name(), TAG_LABEL, "Notifications: %notifications%");
      m_label->replace_token("%notifications%", m_empty_notifications ? "0" : "");
    }

    assert(static_cast<bool>(m_label));
  }

  /**
   * Update module contents
   */
  bool github_module::update() {
    string content{m_http->get("https://api.github.com/notifications?access_token=" + m_accesstoken)};
    long response_code{m_http->response_code()};

    switch (response_code) {
      case 200:
        break;
      case 401:
        throw module_error("Bad credentials");
      case 403:
        throw module_error("Maximum number of login attempts exceeded");
      default:
        throw module_error("Unspecified error (" + to_string(response_code) + ")");
    }

    size_t pos{0};
    size_t notifications{0};

    while ((pos = content.find("\"unread\":true", pos + 1)) != string::npos) {
      notifications++;
    }

    if (notifications || m_empty_notifications) {
      m_label->reset_tokens();
      m_label->replace_token("%notifications%", to_string(notifications));
    } else {
      m_label->clear();
    }

    return true;
  }

  /**
   * Build module content
   */
  bool github_module::build(builder* builder, const string& tag) const {
    if (tag == TAG_LABEL) {
      builder->node(m_label);
    } else {
      return false;
    }
    return true;
  }
}

POLYBAR_NS_END
