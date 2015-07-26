all:
	g++ ftrl_train.cpp src/DB/mapDB.h src/Trainer/FTRL.cpp src/Unit/entityUnit.h src/Unit/modelUnit.h src/Utils/utils.cpp src/Utils/utils.h -I . -std=c++0x -o bin/ftrl_train -lpthread
	g++ ftrl_predict.cpp src/DB/mapDB.h src/Trainer/FTRL.cpp src/Unit/entityUnit.h src/Unit/modelUnit.h src/Utils/utils.cpp src/Utils/utils.h -I . -std=c++0x -o bin/ftrl_predict -lpthread
