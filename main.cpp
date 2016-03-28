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

const cocaine::logging::attributes_t& make_attributes() {
    static cocaine::logging::attributes_t attributes = [](){
        cocaine::logging::attributes_t attr;
        attr.push_back(cocaine::logging::attribute_t {std::string("attribute1"), cocaine::dynamic_t("attribute1_value")} );
        attr.push_back(cocaine::logging::attribute_t {std::string("attribute2"), cocaine::dynamic_t("attribute2_value")} );
        attr.push_back(cocaine::logging::attribute_t {std::string("attribute3"), cocaine::dynamic_t("attribute3_value")} );
        attr.push_back(cocaine::logging::attribute_t {std::string("attribute4"), cocaine::dynamic_t("attribute4_value")} );
        attr.push_back(cocaine::logging::attribute_t {std::string("attribute5"), cocaine::dynamic_t("attribute5_value")} );
        attr.push_back(cocaine::logging::attribute_t {std::string("attribute6"), cocaine::dynamic_t(42)} );
        attr.push_back(cocaine::logging::attribute_t {std::string("attribute7"), cocaine::dynamic_t(true)} );
        return attr;
    }();
    return attributes;
}

int main(int argc, char** argv) {

    service_manager_t manager(1), m_manager(1);
    uint iters        = 100;
    unsigned int verbosity = 0;


    worker_t worker(options_t(argc, argv));

    typedef http::event<> http_t;
    worker.on<http_t>("new_ack", [&](http_t::fresh_sender tx, http_t::fresh_receiver rx) {
        std::string service   = "logging::v2";
		auto logger = manager.create<cocaine::io::base_log_tag>(service);
		std::string backend("logger_test_app");
		logger.connect().get();
		auto channel = logger.invoke<cocaine::io::base_log::get>(backend).get();
		auto l_tx = std::move(channel.tx);
	    auto l_rx = std::move(channel.rx);
		std::vector<task<boost::optional<bool>>::future_type> futures;
		for (uint id = 0; id < iters; ++id) {
			l_tx = l_tx.send<cocaine::io::named_log::emit_ack>(verbosity, "log string", make_attributes()).get();
			futures.emplace_back(l_rx.recv());
		}
		for (auto& future : futures) {
	       future.get();
		}
        http_response_t rs;
        rs.code = 200;
        tx.send(std::move(rs)).get()
            .send("Hello from C++").get();
    });

    typedef http::event<> http_t;
    worker.on<http_t>("new", [&](http_t::fresh_sender tx, http_t::fresh_receiver rx) {
        std::string service   = "logging::v2";
        auto logger = manager.create<cocaine::io::base_log_tag>(service);
        std::string backend("logger_test_app");
        logger.connect().get();
        auto channel = logger.invoke<cocaine::io::base_log::get>(backend).get();
        auto l_tx = std::move(channel.tx);
        auto l_rx = std::move(channel.rx);
        for (uint id = 0; id < iters; ++id) {
            l_tx = l_tx.send<cocaine::io::named_log::emit>(verbosity, "log string", make_attributes()).get();
        }
        http_response_t rs;
        rs.code = 200;
        tx.send(std::move(rs)).get()
        .send("Hello from C++").get();
    });

    worker.on<http_t>("new_plain", [&](http_t::fresh_sender tx, http_t::fresh_receiver rx) {
        std::string service   = "logging::v2";
        auto logger = manager.create<cocaine::io::base_log_tag>(service);
        std::string backend("logger_test_app");
        logger.connect().get();
        for (uint id = 0; id < iters; ++id) {
            logger.invoke<cocaine::io::base_log::emit>(verbosity, backend, "log_string", make_attributes()).get();
        }
        http_response_t rs;
        rs.code = 200;
        tx.send(std::move(rs)).get()
        .send("Hello from C++").get();
    });

    worker.on<http_t>("new_plain_ack", [&](http_t::fresh_sender tx, http_t::fresh_receiver rx) {
        std::string service   = "logging::v2";
        auto logger = manager.create<cocaine::io::base_log_tag>(service);
        std::string backend("logger_test_app");
        logger.connect().get();
        std::vector<task<bool>::future_type> futures;
        for (uint id = 0; id < iters; ++id) {
            futures.emplace_back(logger.invoke<cocaine::io::base_log::emit_ack>(verbosity, backend, "log_string", make_attributes()));
        }
        for (auto& future : futures) {
            future.get();
        }
        http_response_t rs;
        rs.code = 200;
        tx.send(std::move(rs)).get()
        .send("Hello from C++").get();
    });


    worker.on<http_t>("old", [&](http_t::fresh_sender tx, http_t::fresh_receiver rx) {
        std::string service   = "logging";
        auto logger = manager.create<cocaine::io::base_log_tag>(service);
        std::string backend("logger_test_app");
        logger.connect().get();
        auto channel = logger.invoke<cocaine::io::base_log::get>(backend).get();
        auto l_tx = std::move(channel.tx);
        auto l_rx = std::move(channel.rx);
        std::vector<task<boost::optional<bool>>::future_type> futures;
        cocaine::logging::attributes_t attributes;
        attributes.push_back(cocaine::logging::attribute_t {std::string("attribute1"), cocaine::dynamic_t("attribute1_value")} );
        attributes.push_back(cocaine::logging::attribute_t {std::string("attribute2"), cocaine::dynamic_t("attribute2_value")} );
        attributes.push_back(cocaine::logging::attribute_t {std::string("attribute3"), cocaine::dynamic_t("attribute3_value")} );
        attributes.push_back(cocaine::logging::attribute_t {std::string("attribute4"), cocaine::dynamic_t("attribute4_value")} );
        attributes.push_back(cocaine::logging::attribute_t {std::string("attribute5"), cocaine::dynamic_t("attribute5_value")} );
        attributes.push_back(cocaine::logging::attribute_t {std::string("attribute6"), cocaine::dynamic_t(42)} );
        attributes.push_back(cocaine::logging::attribute_t {std::string("attribute7"), cocaine::dynamic_t(true)} );
        for (uint id = 0; id < iters; ++id) {
            l_tx = l_tx.send<cocaine::io::named_log::emit_ack>(verbosity, "log string", attributes).get();
            futures.emplace_back(l_rx.recv());
        }
        for (auto& future : futures) {
            future.get();
        }
        http_response_t rs;
        rs.code = 200;
        tx.send(std::move(rs)).get()
        .send("Hello from C++").get();
    });

    worker.on<http_t>("increase_verbosity", [&](http_t::fresh_sender tx, http_t::fresh_receiver rx) {
        verbosity++;
        http_response_t rs;
        rs.code = 200;
        tx.send(std::move(rs)).get()
            .send(std::to_string(verbosity)).get();
    });

    worker.on<http_t>("decrease_verbosity", [&](http_t::fresh_sender tx, http_t::fresh_receiver rx) {
        verbosity--;
        http_response_t rs;
        rs.code = 200;
        tx.send(std::move(rs)).get()
            .send(std::to_string(verbosity)).get();
    });


    return worker.run();
}
