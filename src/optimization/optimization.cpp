#include "optimization/optimization.hpp"

using namespace arkoi;

void OptimizationManager::optimize(std::vector<CFG> &cfgs) {
    while (true) {
        auto changed = false;

        for (const auto &pass: _iterative_passes) {
            std::for_each(cfgs.begin(), cfgs.end(), [&](auto &cfg) {
                changed |= pass->new_cfg(cfg);

                cfg.depth_first_search([&](BasicBlock &block) {
                    changed |= pass->new_block(block);
                });
            });
        }

        if (!changed) break;
    }
    
    for (const auto &pass: _single_passes) {
        std::for_each(cfgs.begin(), cfgs.end(), [&](auto &cfg) {
            pass->new_cfg(cfg);

            cfg.depth_first_search([&](BasicBlock &block) {
                pass->new_block(block);
            });
        });
    }
}
