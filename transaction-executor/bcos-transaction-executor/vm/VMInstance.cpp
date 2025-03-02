#include "VMInstance.h"

void bcos::transaction_executor::VMInstance::ReleaseEVMC::operator()(evmc_vm* ptr) const noexcept
{
    if (ptr != nullptr)
    {
        ptr->destroy(ptr);
    }
}

evmc_revision bcos::transaction_executor::toRevision(VMSchedule const& _schedule)
{
    if (_schedule.enablePairs)
    {
        return EVMC_PARIS;
    }
    return EVMC_LONDON;
}

bcos::transaction_executor::EVMCResult bcos::transaction_executor::VMInstance::execute(
    const struct evmc_host_interface* host, struct evmc_host_context* context, evmc_revision rev,
    const evmc_message* msg, const uint8_t* code, size_t codeSize)
{
    return std::visit(overloaded{[&](EVMC_VM const& instance) {
                                     return EVMCResult(instance->execute(
                                         instance.get(), host, context, rev, msg, code, codeSize));
                                 },
                          [&](EVMC_ANALYSIS_RESULT const& instance) {
                              auto state = evmone::advanced::AdvancedExecutionState(*msg, rev,
                                  *host, context, std::basic_string_view<uint8_t>(code, codeSize));
                              return EVMCResult(evmone::advanced::execute(state, *instance));
                          }},
        m_instance);
}

void bcos::transaction_executor::VMInstance::enableDebugOutput() {}
