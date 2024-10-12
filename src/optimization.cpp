#include "optimization.hpp"

void OptimizationManager::optimize(std::vector<CFG> &cfgs) {
    for (auto &cfg: cfgs) {
        for (const auto &pass: _passes) {
            pass->new_cfg(cfg);
        }

        cfg.depth_first_search([&](BasicBlock &block) {
            for (const auto &pass: _passes) {
                pass->new_block(block);
            }

            for (const auto &pass: _passes) {
                for (auto &instruction: block.instructions()) {
                    pass->instruction(instruction);
                }
            }
        });
    }
}
