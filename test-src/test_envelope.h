#include <gtest/gtest.h>
#include "../src/envelope.h"

namespace {

using namespace std;
using namespace txmpp;
using namespace tyrion;

static const string static_jid = "test@example.org";
static const string static_id = "id";
static const string static_iq_id = "iq_id";
static const string static_type = "type";
static const string static_user = "user";
static const string static_group = "group";
static const string static_output = "Some Output";
static const string static_error = "Some Error";
static const int static_code = 4;
static const int static_timeout = 101;

TEST(EnvelopeTest, Basic) {
  Envelope e;

  // retry
  EXPECT_EQ(0, e.retry());
  EXPECT_EQ(1, e.Retry());
  EXPECT_EQ(2, e.Retry());
  EXPECT_EQ(4, e.Retry());
  EXPECT_EQ(4, e.retry());
  e.set_retry(0);
  EXPECT_EQ(0, e.retry());

  // jid
  EXPECT_EQ(Jid(STR_EMPTY).Str(), e.jid().Str());
  e.set_jid(static_jid);
  EXPECT_EQ(static_jid, e.jid().Str());
  e.set_jid(Jid(static_jid));
  EXPECT_EQ(static_jid, e.jid().Str());

  // basic get/set
  EXPECT_EQ(STR_EMPTY, e.id());
  EXPECT_EQ(STR_EMPTY, e.iq_id());
  EXPECT_EQ(STR_EMPTY, e.type());
  EXPECT_EQ(STR_EMPTY, e.user());
  EXPECT_EQ(STR_EMPTY, e.group());
  EXPECT_EQ(0, e.code());
  e.set_id(static_id);
  e.set_iq_id(static_iq_id);
  e.set_type(static_type);
  e.set_user(static_user);
  e.set_group(static_group);
  e.set_code(static_code);
  EXPECT_EQ(static_id, e.id());
  EXPECT_EQ(static_iq_id, e.iq_id());
  EXPECT_EQ(static_type, e.type());
  EXPECT_EQ(static_user, e.user());
  EXPECT_EQ(static_group, e.group());
  EXPECT_EQ(static_code, e.code());

  // output/error
  EXPECT_EQ(STR_EMPTY, e.output());
  EXPECT_EQ(STR_EMPTY, e.error());
  e.set_output(static_output);
  e.set_error(static_error);
  EXPECT_EQ(static_output, e.output());
  EXPECT_EQ(static_error, e.error());
  e.append_output(static_user);
  e.append_error(static_group);
  EXPECT_EQ(static_output + static_user, e.output());
  EXPECT_EQ(static_error + static_group, e.error());

  // timeout
  EXPECT_EQ(PROCESS_TIMEOUT, e.timeout());
  e.set_timeout(static_timeout);
  EXPECT_EQ(static_timeout, e.timeout());
  e.set_timeout(0);
  EXPECT_EQ(PROCESS_TIMEOUT, e.timeout());
}

}  // namespace
