#include "opt/optimization.hpp"

using namespace arkoi::opt;

void PassManager::optimize(mid::Module &module) {
    auto &functions = module.functions();

    while (true) {
        auto changed = false;

        for (const auto &pass: _passes) {
            std::for_each(functions.begin(), functions.end(), [&](auto &function) {
                changed |= pass->new_function(function);

                function.depth_first_search([&](mid::BasicBlock &block) {
                    changed |= pass->new_block(block);
                });
            });
        }

        if (!changed) break;
    }
}
