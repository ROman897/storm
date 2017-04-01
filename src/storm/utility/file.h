/** 
 * @file:   file.h
 * @author: Sebastian Junges
 *
 * @since October 7, 2014
 */

#ifndef STORM_UTILITY_FILE_H_
#define STORM_UTILITY_FILE_H_

#include <iostream>

#include "storm/utility/macros.h"
#include "storm/exceptions/FileIoException.h"

namespace storm {
    namespace utility {

        /*!
         * Open the given file for writing.
         *
         * @param filename Path and name of the file to be tested.
         * @param filestream Contains the file handler afterwards.
         * @param append If true, the new content is appended instead of clearing the existing content.
         */
        inline void openFile(std::string const& filepath, std::ofstream& filestream, bool append = false) {
            if (append) {
                filestream.open(filepath, std::ios::app);
            } else {
                filestream.open(filepath);
            }
            STORM_LOG_THROW(filestream, storm::exceptions::FileIoException , "Could not open file " << filepath << ".");
            STORM_PRINT_AND_LOG("Write to file " << filepath << "." << std::endl);
        }

        /*!
         * Open the given file for reading.
         *
         * @param filename Path and name of the file to be tested.
         * @param filestream Contains the file handler afterwards.
         */
        inline void openFile(std::string const& filepath, std::ifstream& filestream) {
            filestream.open(filepath);
            STORM_LOG_THROW(filestream, storm::exceptions::FileIoException , "Could not open file " << filepath << ".");
        }

        /*!
         * Close the given file after writing.
         *
         * @param filestream Contains the file handler to close.
         */
        inline void closeFile(std::ofstream& stream) {
            stream.close();
        }

        /*!
         * Close the given file after reading.
         *
         * @param filestream Contains the file handler to close.
         */
        inline void closeFile(std::ifstream& stream) {
            stream.close();
        }

        /*!
         * Tests whether the given file exists and is readable.
         *
         * @param filename Path and name of the file to be tested.
         * @return True iff the file exists and is readable.
         */
        inline bool fileExistsAndIsReadable(std::string const& filename) {
            // Test by opening an input file stream and testing the stream flags.
            std::ifstream filestream;
            filestream.open(filename);
            return filestream.good();
        }

    }
}

#endif