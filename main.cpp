#include <cocaine/common.hpp>
#include <cocaine/framework/worker.hpp>
#include <cocaine/framework/worker/http.hpp>
#include <cocaine/framework/service.hpp>
#include <cocaine/framework/manager.hpp>
#include <cocaine/traits/optional.hpp>
#include <cocaine/traits/attribute.hpp>
#include <cocaine/traits/tuple.hpp>
#include <cocaine/traits/vector.hpp>
#include <cocaine/traits/enum.hpp>
#include <cocaine/idl/logging_v2.hpp>
#include <cocaine/traits/error_code.hpp>
#include <tuple>
#include <string>

using namespace cocaine::framework;
using namespace cocaine::framework::worker;

int main(int argc, char** argv) {



    service_manager_t manager(1), m_manager(1);
    uint iters        = 100;
    std::string service   = "logging::v2";


    auto logger = manager.create<cocaine::io::base_log_tag>(service);
    std::string backend("logger_test_app");
    logger.connect().get();
    worker_t worker(options_t(argc, argv));

    typedef http::event<> http_t;
    worker.on<http_t>("http", [&](http_t::fresh_sender tx, http_t::fresh_receiver){
	auto future = logger.invoke<cocaine::io::base_log::get>(backend).then(
		[&](task<channel<cocaine::io::base_log::get>>::future_move_type fut) {
			auto channel = fut.get();
			auto tx = std::move(channel.tx);
    			auto rx = std::move(channel.rx);
			std::vector<task<boost::optional<bool>>::future_type> futures;
			cocaine::logging::attributes_t attributes;
			attributes.push_back(cocaine::logging::attribute_t{std::string("attribute1"), cocaine::dynamic_t("attribute1_value")});
			attributes.push_back(cocaine::logging::attribute_t{std::string("attribute2"), cocaine::dynamic_t("attribute2_value")});
			attributes.push_back(cocaine::logging::attribute_t{std::string("attribute3"), cocaine::dynamic_t("attribute3_value")});
			attributes.push_back(cocaine::logging::attribute_t{std::string("attribute4"), cocaine::dynamic_t("attribute4_value")});
			attributes.push_back(cocaine::logging::attribute_t{std::string("attribute5"), cocaine::dynamic_t("attribute5_value")});
			for (uint id = 0; id < iters; ++id) {
      				tx = tx.send<cocaine::io::named_log::emit_ack>(2, "log string", attributes).get();
				futures.emplace_back(rx.recv());
			}
			for (auto& future : futures) {
		            future.get();
			}
		}
	);
	future.get();
			
        http_response_t rs;
        rs.code = 200;
        tx.send(std::move(rs)).get()
            .send("Hello from C++").get();
    });

    return worker.run();
}
