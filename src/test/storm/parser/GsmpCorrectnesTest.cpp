#include "gtest/gtest.h"
#include "storm-config.h"

#include "storm/parser/PrismParser.h"
#include "storm/builder/ExplicitModelBuilder.h"
#include "storm/models/sparse/Gsmp.h"


void parseNextEvent(std::ifstream& input, std::string& eventName, storm::storage::SparseMatrix<double>& transitionMatrix) {
	storm::storage::SparseMatrixBuilder<double> matrixBuilder;
	std::string event;
	input >> event;
	eventName = event.substr(1, event.find('=') - 1);

	std::string str;
	uint_fast64_t row = 0;
	while (input >> str) {
		if (str.back() != ':') {
			return;
		}
		uint_fast64_t state = std::stoull(str.substr(0, str.size() - 1));

		input >> str;
		uint_fast64_t toState = std::stoull(str.substr(1, str.find('=')));
		double prob = std::stod(str.substr(str.find('=') + 1, str.size() - 2));
		matrixBuilder.addNextValue(row, toState, prob);
		++row;
	}
	transitionMatrix = matrixBuilder.build();
}

void checkGSMP(std::string const& modelName) {

	std::string completeDir = STORM_TEST_RESOURCES_DIR "/gsmp/" + modelName + "/";
	// STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, completeDir + modelName + ".pm");
	storm::prism::Program program = storm::parser::PrismParser::parse(completeDir + modelName + ".pm");
	// STORM_LOG_WARN("directory is: " << completeDir << modelName << ".pm");
 //    program.checkValidity();
 //    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitModelBuilder<double>(program).build();
 //    ASSERT_TRUE(model->isOfType(storm::models::ModelType::Gsmp));
 //    auto gsmp = *model->as<storm::models::sparse::Gsmp<double>>();

 //    std::ifstream input(completeDir + "result");
 //    std::string eventName;
 //    storm::storage::SparseMatrix<double> transitionMatrix;

 //    std::string str;
 //    while(str != "GSMP") {
 //    	input >> str;
 //    }
 //    input >> str;
 //    input >> str;
 //    uint_fast64_t numberOfEvents = stoull(str);
 //    input >> str;
 //    input >> str;

 //    for (uint_fast64_t i = 0; i < numberOfEvents; ++i) {
 //    	parseNextEvent(input, eventName, transitionMatrix);
 //    	auto myTransitionMatrix = gsmp.getTransitionMatrixForEvent(eventName);
 //    	ASSERT_TRUE(gsmp.hasEvent(eventName));
 //    	ASSERT_EQ(myTransitionMatrix, transitionMatrix);
 //    }

}

TEST(GSMP, TestAgainstWorkingImplementation) {

	std::vector<std::string> testingModels = {"queue_G|G|1|n_modular"};
	for (std::string const& model : testingModels) {
		checkGSMP(model);
	}
}