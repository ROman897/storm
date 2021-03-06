#include "initialize.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/DebugSettings.h"

#include <iostream>
#include <fstream>

namespace storm {
    namespace utility {

        void initializeLogger() {
            l3pp::Logger::initialize();
            // By default output to std::cout
            l3pp::SinkPtr sink = l3pp::StreamSink::create(std::cout);
            l3pp::Logger::getRootLogger()->addSink(sink);
            // Default to warn, set by user to something else
            l3pp::Logger::getRootLogger()->setLevel(l3pp::LogLevel::WARN);

            l3pp::FormatterPtr fptr = l3pp::makeTemplateFormatter(
                    l3pp::FieldStr<l3pp::Field::LogLevel, 5, l3pp::Justification::LEFT>(),
                    " (", l3pp::FieldStr<l3pp::Field::FileName>(), ':', l3pp::FieldStr<l3pp::Field::Line>(), "): ",
                    l3pp::FieldStr<l3pp::Field::Message>(), '\n'
                );
            sink->setFormatter(fptr);
        }

        void setUp() {
            initializeLogger();
            std::cout.precision(10);
        }

        void cleanUp() {
            // Intentionally left empty.
        }

        void setLogLevel(l3pp::LogLevel level) {
            l3pp::Logger::getRootLogger()->setLevel(level);
            if (level <= l3pp::LogLevel::DEBUG) {
#ifdef STORM_LOG_DISABLE_DEBUG
                std::cout << "***** warning ***** requested loglevel is not compiled\n";
#endif
            }
        }

        void initializeFileLogging() {
            if (storm::settings::getModule<storm::settings::modules::DebugSettings>().isLogfileSet()) {
                std::string logFileName = storm::settings::getModule<storm::settings::modules::DebugSettings>().getLogfilename();
                l3pp::SinkPtr sink = l3pp::FileSink::create(logFileName);
                l3pp::Logger::getRootLogger()->addSink(sink);
            }
        }

    }
}
