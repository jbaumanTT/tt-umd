// SPDX-FileCopyrightText: © 2025 Tenstorrent Inc.
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <cstdint>

#include "umd/device/types/arch.hpp"
#include "umd/device/types/tlb.hpp"

namespace tt::umd {

/**
 * Base class for TLB handles that provides a common interface
 * for both hardware (silicon) and simulation implementations.
 */
class TlbHandle {
public:
    virtual ~TlbHandle() noexcept = default;

    /**
     * Configures the TLB with the provided configuration.
     *
     * @param new_config The new configuration for the TLB.
     */
    virtual void configure(const tlb_data& new_config) = 0;

    /**
     * Returns the base mapped address of the TLB.
     */
    uint8_t* get_base() const { return tlb_base_; }

    /**
     * Returns the size of the TLB (the hardware aperture). Address alignment is derived from this.
     */
    size_t get_size() const { return tlb_size_; }

    /**
     * Returns the number of bytes actually mapped into the host address space. This is <= get_size();
     * it is smaller when the aperture is larger than the device memory backed at the endpoint and the
     * unmapped tail is deliberately left without a page-table entry. Access bounds checks use this.
     */
    size_t get_mmap_size() const { return mmap_size_ != 0 ? mmap_size_ : tlb_size_; }

    /**
     * Returns the current configuration of the TLB.
     */
    const tlb_data& get_config() const { return tlb_config_; }

    /**
     * Returns the TLB mapping type (UC or WC).
     */
    TlbMapping get_tlb_mapping() const { return tlb_mapping_; }

    /**
     * Returns the TLB ID, representing index of TLB in BAR0.
     */
    int get_tlb_id() const { return tlb_id_; }

    virtual tt::ARCH get_arch() const = 0;

protected:
    /**
     * Protected default constructor - only derived classes can construct.
     */
    TlbHandle() = default;

    int tlb_id_ = 0;
    uint8_t* tlb_base_ = nullptr;
    size_t tlb_size_ = 0;
    size_t mmap_size_ = 0;  // Host-mapped bytes; 0 means the full aperture (tlb_size_).
    tlb_data tlb_config_{};
    TlbMapping tlb_mapping_ = TlbMapping::UC;

private:
    /**
     * Free any TLB resources. Called by destructor.
     * Implemented by derived classes.
     */
    virtual void free_tlb() noexcept = 0;
};

}  // namespace tt::umd
