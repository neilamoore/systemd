/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "architecture.h"
#include "glyph-util.h"
#include "gpt.h"
#include "log.h"
#include "pretty-print.h"
#include "strv.h"
#include "terminal-util.h"
#include "tests.h"

TEST(gpt_types_against_architectures) {
        int r;

        /* Dumps a table indicating for which architectures we know we have matching GPT partition
         * types. Also validates whether we can properly categorize the entries. */

        FOREACH_STRING(prefix, "root-", "usr-")
                for (Architecture a = 0; a < _ARCHITECTURE_MAX; a++)
                        FOREACH_STRING(suffix, "", "-verity", "-verity-sig") {
                                _cleanup_free_ char *joined = NULL;
                                GptPartitionType type;

                                joined = strjoin(prefix, architecture_to_string(a), suffix);
                                if (!joined)
                                        return (void) log_oom();

                                r = gpt_partition_type_from_string(joined, &type);
                                if (r < 0) {
                                        printf("%s %s\n", RED_CROSS_MARK(), joined);
                                        continue;
                                }

                                printf("%s %s\n", GREEN_CHECK_MARK(), joined);

                                if (streq(prefix, "root-") && streq(suffix, ""))
                                        assert_se(type.designator == PARTITION_ROOT);
                                if (streq(prefix, "root-") && streq(suffix, "-verity"))
                                        assert_se(type.designator == PARTITION_ROOT_VERITY);
                                if (streq(prefix, "usr-") && streq(suffix, ""))
                                        assert_se(type.designator == PARTITION_USR);
                                if (streq(prefix, "usr-") && streq(suffix, "-verity"))
                                        assert_se(type.designator == PARTITION_USR_VERITY);

                                assert_se(type.arch == a);
                        }
}

DEFINE_TEST_MAIN(LOG_INFO);
