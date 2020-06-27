#include "Token.h"
#include "Nodes.h"
#include "Error.h"
#include "Platform.h"

#include <functional>

class API Parser {
public:
	Parser();

	struct Result {
		template<typename T>
		Node* record(T* result) {
			if (strcmp(typeid(T).name(), "struct Parser::Result") == 0) {
				Result* res = (Result*)result;
				if (res->error != nullptr) {
					error = res->error;
				}

				return res->node;
			}

			return (Node*)result;
		}

		void record_advance() {
			count++;
		}

		Result* success(Node* node) {
			this->node = node;
			return this;
		}

		Result* failure(Error* error) {
			this->error = error;
			return this;
		}

		Error* error = nullptr;
		Node* node = nullptr;
		unsigned int count = 0;
	};

	Result* parse();
	void traverse(Node* node, unsigned int depth);
	Token* advance();
	Result* factor();
	Result* term();
	Result* expr();
	Result* atom();
	Result* arithm();
	Result* power();
	Result* compare();
	
	Result* binary_operation(
		std::function<Result*()> fna, 
		const std::vector<Token::Type>& operations, 
		const std::vector<std::string>& values, 
		std::function<Result* ()> fnb = nullptr
	);

	inline void setTokens(const std::vector<Token*>& tokens) {
		this->tokens = tokens;
	}

	std::vector<Token*> tokens;
	Token* current_token;
	size_t index;
	bool debug;
};