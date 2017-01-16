#define	a0dalloc JEMALLOC_N(a0dalloc)
#define	a0malloc JEMALLOC_N(a0malloc)
#define	arena_aalloc JEMALLOC_N(arena_aalloc)
#define	arena_alloc_junk_small JEMALLOC_N(arena_alloc_junk_small)
#define	arena_basic_stats_merge JEMALLOC_N(arena_basic_stats_merge)
#define	arena_bin_index JEMALLOC_N(arena_bin_index)
#define	arena_bin_info JEMALLOC_N(arena_bin_info)
#define	arena_boot JEMALLOC_N(arena_boot)
#define	arena_choose JEMALLOC_N(arena_choose)
#define	arena_choose_hard JEMALLOC_N(arena_choose_hard)
#define	arena_choose_impl JEMALLOC_N(arena_choose_impl)
#define	arena_cleanup JEMALLOC_N(arena_cleanup)
#define	arena_dalloc JEMALLOC_N(arena_dalloc)
#define	arena_dalloc_bin_junked_locked JEMALLOC_N(arena_dalloc_bin_junked_locked)
#define	arena_dalloc_junk_small JEMALLOC_N(arena_dalloc_junk_small)
#define	arena_dalloc_promoted JEMALLOC_N(arena_dalloc_promoted)
#define	arena_dalloc_small JEMALLOC_N(arena_dalloc_small)
#define	arena_decay_tick JEMALLOC_N(arena_decay_tick)
#define	arena_decay_ticks JEMALLOC_N(arena_decay_ticks)
#define	arena_decay_time_default_get JEMALLOC_N(arena_decay_time_default_get)
#define	arena_decay_time_default_set JEMALLOC_N(arena_decay_time_default_set)
#define	arena_decay_time_get JEMALLOC_N(arena_decay_time_get)
#define	arena_decay_time_set JEMALLOC_N(arena_decay_time_set)
#define	arena_destroy JEMALLOC_N(arena_destroy)
#define	arena_dss_prec_get JEMALLOC_N(arena_dss_prec_get)
#define	arena_dss_prec_set JEMALLOC_N(arena_dss_prec_set)
#define	arena_extent_alloc_large JEMALLOC_N(arena_extent_alloc_large)
#define	arena_extent_cache_alloc JEMALLOC_N(arena_extent_cache_alloc)
#define	arena_extent_cache_dalloc JEMALLOC_N(arena_extent_cache_dalloc)
#define	arena_extent_cache_maybe_insert JEMALLOC_N(arena_extent_cache_maybe_insert)
#define	arena_extent_cache_maybe_remove JEMALLOC_N(arena_extent_cache_maybe_remove)
#define	arena_extent_dalloc_large JEMALLOC_N(arena_extent_dalloc_large)
#define	arena_extent_ralloc_large_expand JEMALLOC_N(arena_extent_ralloc_large_expand)
#define	arena_extent_ralloc_large_shrink JEMALLOC_N(arena_extent_ralloc_large_shrink)
#define	arena_extent_sn_next JEMALLOC_N(arena_extent_sn_next)
#define	arena_get JEMALLOC_N(arena_get)
#define	arena_ichoose JEMALLOC_N(arena_ichoose)
#define	arena_ind_get JEMALLOC_N(arena_ind_get)
#define	arena_init JEMALLOC_N(arena_init)
#define	arena_internal_add JEMALLOC_N(arena_internal_add)
#define	arena_internal_get JEMALLOC_N(arena_internal_get)
#define	arena_internal_sub JEMALLOC_N(arena_internal_sub)
#define	arena_malloc JEMALLOC_N(arena_malloc)
#define	arena_malloc_hard JEMALLOC_N(arena_malloc_hard)
#define	arena_maybe_purge JEMALLOC_N(arena_maybe_purge)
#define	arena_migrate JEMALLOC_N(arena_migrate)
#define	arena_new JEMALLOC_N(arena_new)
#define	arena_nthreads_dec JEMALLOC_N(arena_nthreads_dec)
#define	arena_nthreads_get JEMALLOC_N(arena_nthreads_get)
#define	arena_nthreads_inc JEMALLOC_N(arena_nthreads_inc)
#define	arena_palloc JEMALLOC_N(arena_palloc)
#define	arena_postfork_child JEMALLOC_N(arena_postfork_child)
#define	arena_postfork_parent JEMALLOC_N(arena_postfork_parent)
#define	arena_prefork0 JEMALLOC_N(arena_prefork0)
#define	arena_prefork1 JEMALLOC_N(arena_prefork1)
#define	arena_prefork2 JEMALLOC_N(arena_prefork2)
#define	arena_prefork3 JEMALLOC_N(arena_prefork3)
#define	arena_prof_accum JEMALLOC_N(arena_prof_accum)
#define	arena_prof_accum_impl JEMALLOC_N(arena_prof_accum_impl)
#define	arena_prof_accum_locked JEMALLOC_N(arena_prof_accum_locked)
#define	arena_prof_promote JEMALLOC_N(arena_prof_promote)
#define	arena_prof_tctx_get JEMALLOC_N(arena_prof_tctx_get)
#define	arena_prof_tctx_reset JEMALLOC_N(arena_prof_tctx_reset)
#define	arena_prof_tctx_set JEMALLOC_N(arena_prof_tctx_set)
#define	arena_purge JEMALLOC_N(arena_purge)
#define	arena_ralloc JEMALLOC_N(arena_ralloc)
#define	arena_ralloc_no_move JEMALLOC_N(arena_ralloc_no_move)
#define	arena_reset JEMALLOC_N(arena_reset)
#define	arena_salloc JEMALLOC_N(arena_salloc)
#define	arena_sdalloc JEMALLOC_N(arena_sdalloc)
#define	arena_set JEMALLOC_N(arena_set)
#define	arena_slab_regind JEMALLOC_N(arena_slab_regind)
#define	arena_stats_merge JEMALLOC_N(arena_stats_merge)
#define	arena_tcache_fill_small JEMALLOC_N(arena_tcache_fill_small)
#define	arena_tdata_get JEMALLOC_N(arena_tdata_get)
#define	arena_tdata_get_hard JEMALLOC_N(arena_tdata_get_hard)
#define	arenas JEMALLOC_N(arenas)
#define	arenas_tdata_cleanup JEMALLOC_N(arenas_tdata_cleanup)
#define	atomic_add_p JEMALLOC_N(atomic_add_p)
#define	atomic_add_u JEMALLOC_N(atomic_add_u)
#define	atomic_add_u32 JEMALLOC_N(atomic_add_u32)
#define	atomic_add_u64 JEMALLOC_N(atomic_add_u64)
#define	atomic_add_zu JEMALLOC_N(atomic_add_zu)
#define	atomic_cas_p JEMALLOC_N(atomic_cas_p)
#define	atomic_cas_u JEMALLOC_N(atomic_cas_u)
#define	atomic_cas_u32 JEMALLOC_N(atomic_cas_u32)
#define	atomic_cas_u64 JEMALLOC_N(atomic_cas_u64)
#define	atomic_cas_zu JEMALLOC_N(atomic_cas_zu)
#define	atomic_sub_p JEMALLOC_N(atomic_sub_p)
#define	atomic_sub_u JEMALLOC_N(atomic_sub_u)
#define	atomic_sub_u32 JEMALLOC_N(atomic_sub_u32)
#define	atomic_sub_u64 JEMALLOC_N(atomic_sub_u64)
#define	atomic_sub_zu JEMALLOC_N(atomic_sub_zu)
#define	atomic_write_p JEMALLOC_N(atomic_write_p)
#define	atomic_write_u JEMALLOC_N(atomic_write_u)
#define	atomic_write_u32 JEMALLOC_N(atomic_write_u32)
#define	atomic_write_u64 JEMALLOC_N(atomic_write_u64)
#define	atomic_write_zu JEMALLOC_N(atomic_write_zu)
#define	b0get JEMALLOC_N(b0get)
#define	base_alloc JEMALLOC_N(base_alloc)
#define	base_boot JEMALLOC_N(base_boot)
#define	base_delete JEMALLOC_N(base_delete)
#define	base_extent_hooks_get JEMALLOC_N(base_extent_hooks_get)
#define	base_extent_hooks_set JEMALLOC_N(base_extent_hooks_set)
#define	base_ind_get JEMALLOC_N(base_ind_get)
#define	base_new JEMALLOC_N(base_new)
#define	base_postfork_child JEMALLOC_N(base_postfork_child)
#define	base_postfork_parent JEMALLOC_N(base_postfork_parent)
#define	base_prefork JEMALLOC_N(base_prefork)
#define	base_stats_get JEMALLOC_N(base_stats_get)
#define	bitmap_full JEMALLOC_N(bitmap_full)
#define	bitmap_get JEMALLOC_N(bitmap_get)
#define	bitmap_info_init JEMALLOC_N(bitmap_info_init)
#define	bitmap_init JEMALLOC_N(bitmap_init)
#define	bitmap_set JEMALLOC_N(bitmap_set)
#define	bitmap_sfu JEMALLOC_N(bitmap_sfu)
#define	bitmap_size JEMALLOC_N(bitmap_size)
#define	bitmap_unset JEMALLOC_N(bitmap_unset)
#define	bootstrap_calloc JEMALLOC_N(bootstrap_calloc)
#define	bootstrap_free JEMALLOC_N(bootstrap_free)
#define	bootstrap_malloc JEMALLOC_N(bootstrap_malloc)
#define	bt_init JEMALLOC_N(bt_init)
#define	buferror JEMALLOC_N(buferror)
#define	ckh_count JEMALLOC_N(ckh_count)
#define	ckh_delete JEMALLOC_N(ckh_delete)
#define	ckh_insert JEMALLOC_N(ckh_insert)
#define	ckh_iter JEMALLOC_N(ckh_iter)
#define	ckh_new JEMALLOC_N(ckh_new)
#define	ckh_pointer_hash JEMALLOC_N(ckh_pointer_hash)
#define	ckh_pointer_keycomp JEMALLOC_N(ckh_pointer_keycomp)
#define	ckh_remove JEMALLOC_N(ckh_remove)
#define	ckh_search JEMALLOC_N(ckh_search)
#define	ckh_string_hash JEMALLOC_N(ckh_string_hash)
#define	ckh_string_keycomp JEMALLOC_N(ckh_string_keycomp)
#define	ctl_boot JEMALLOC_N(ctl_boot)
#define	ctl_bymib JEMALLOC_N(ctl_bymib)
#define	ctl_byname JEMALLOC_N(ctl_byname)
#define	ctl_nametomib JEMALLOC_N(ctl_nametomib)
#define	ctl_postfork_child JEMALLOC_N(ctl_postfork_child)
#define	ctl_postfork_parent JEMALLOC_N(ctl_postfork_parent)
#define	ctl_prefork JEMALLOC_N(ctl_prefork)
#define	decay_ticker_get JEMALLOC_N(decay_ticker_get)
#define	dss_prec_names JEMALLOC_N(dss_prec_names)
#define	extent_active_get JEMALLOC_N(extent_active_get)
#define	extent_active_set JEMALLOC_N(extent_active_set)
#define	extent_ad_comp JEMALLOC_N(extent_ad_comp)
#define	extent_addr_get JEMALLOC_N(extent_addr_get)
#define	extent_addr_randomize JEMALLOC_N(extent_addr_randomize)
#define	extent_addr_set JEMALLOC_N(extent_addr_set)
#define	extent_alloc JEMALLOC_N(extent_alloc)
#define	extent_alloc_cache JEMALLOC_N(extent_alloc_cache)
#define	extent_alloc_cache_locked JEMALLOC_N(extent_alloc_cache_locked)
#define	extent_alloc_dss JEMALLOC_N(extent_alloc_dss)
#define	extent_alloc_mmap JEMALLOC_N(extent_alloc_mmap)
#define	extent_alloc_wrapper JEMALLOC_N(extent_alloc_wrapper)
#define	extent_arena_get JEMALLOC_N(extent_arena_get)
#define	extent_arena_set JEMALLOC_N(extent_arena_set)
#define	extent_base_get JEMALLOC_N(extent_base_get)
#define	extent_before_get JEMALLOC_N(extent_before_get)
#define	extent_boot JEMALLOC_N(extent_boot)
#define	extent_commit_wrapper JEMALLOC_N(extent_commit_wrapper)
#define	extent_committed_get JEMALLOC_N(extent_committed_get)
#define	extent_committed_set JEMALLOC_N(extent_committed_set)
#define	extent_dalloc JEMALLOC_N(extent_dalloc)
#define	extent_dalloc_cache JEMALLOC_N(extent_dalloc_cache)
#define	extent_dalloc_gap JEMALLOC_N(extent_dalloc_gap)
#define	extent_dalloc_mmap JEMALLOC_N(extent_dalloc_mmap)
#define	extent_dalloc_wrapper JEMALLOC_N(extent_dalloc_wrapper)
#define	extent_dalloc_wrapper_try JEMALLOC_N(extent_dalloc_wrapper_try)
#define	extent_decommit_wrapper JEMALLOC_N(extent_decommit_wrapper)
#define	extent_dss_boot JEMALLOC_N(extent_dss_boot)
#define	extent_dss_mergeable JEMALLOC_N(extent_dss_mergeable)
#define	extent_dss_prec_get JEMALLOC_N(extent_dss_prec_get)
#define	extent_dss_prec_set JEMALLOC_N(extent_dss_prec_set)
#define	extent_heap_empty JEMALLOC_N(extent_heap_empty)
#define	extent_heap_first JEMALLOC_N(extent_heap_first)
#define	extent_heap_insert JEMALLOC_N(extent_heap_insert)
#define	extent_heap_new JEMALLOC_N(extent_heap_new)
#define	extent_heap_remove JEMALLOC_N(extent_heap_remove)
#define	extent_heap_remove_first JEMALLOC_N(extent_heap_remove_first)
#define	extent_hooks_default JEMALLOC_N(extent_hooks_default)
#define	extent_hooks_get JEMALLOC_N(extent_hooks_get)
#define	extent_hooks_set JEMALLOC_N(extent_hooks_set)
#define	extent_in_dss JEMALLOC_N(extent_in_dss)
#define	extent_init JEMALLOC_N(extent_init)
#define	extent_last_get JEMALLOC_N(extent_last_get)
#define	extent_lookup JEMALLOC_N(extent_lookup)
#define	extent_merge_wrapper JEMALLOC_N(extent_merge_wrapper)
#define	extent_past_get JEMALLOC_N(extent_past_get)
#define	extent_prof_tctx_get JEMALLOC_N(extent_prof_tctx_get)
#define	extent_prof_tctx_set JEMALLOC_N(extent_prof_tctx_set)
#define	extent_purge_forced_wrapper JEMALLOC_N(extent_purge_forced_wrapper)
#define	extent_purge_lazy_wrapper JEMALLOC_N(extent_purge_lazy_wrapper)
#define	extent_retained_get JEMALLOC_N(extent_retained_get)
#define	extent_ring_insert JEMALLOC_N(extent_ring_insert)
#define	extent_ring_remove JEMALLOC_N(extent_ring_remove)
#define	extent_size_get JEMALLOC_N(extent_size_get)
#define	extent_size_quantize_ceil JEMALLOC_N(extent_size_quantize_ceil)
#define	extent_size_quantize_floor JEMALLOC_N(extent_size_quantize_floor)
#define	extent_size_set JEMALLOC_N(extent_size_set)
#define	extent_slab_data_get JEMALLOC_N(extent_slab_data_get)
#define	extent_slab_data_get_const JEMALLOC_N(extent_slab_data_get_const)
#define	extent_slab_get JEMALLOC_N(extent_slab_get)
#define	extent_slab_set JEMALLOC_N(extent_slab_set)
#define	extent_sn_comp JEMALLOC_N(extent_sn_comp)
#define	extent_sn_get JEMALLOC_N(extent_sn_get)
#define	extent_sn_set JEMALLOC_N(extent_sn_set)
#define	extent_snad_comp JEMALLOC_N(extent_snad_comp)
#define	extent_split_wrapper JEMALLOC_N(extent_split_wrapper)
#define	extent_usize_get JEMALLOC_N(extent_usize_get)
#define	extent_usize_set JEMALLOC_N(extent_usize_set)
#define	extent_zeroed_get JEMALLOC_N(extent_zeroed_get)
#define	extent_zeroed_set JEMALLOC_N(extent_zeroed_set)
#define	extents_rtree JEMALLOC_N(extents_rtree)
#define	ffs_llu JEMALLOC_N(ffs_llu)
#define	ffs_lu JEMALLOC_N(ffs_lu)
#define	ffs_u JEMALLOC_N(ffs_u)
#define	ffs_u32 JEMALLOC_N(ffs_u32)
#define	ffs_u64 JEMALLOC_N(ffs_u64)
#define	ffs_zu JEMALLOC_N(ffs_zu)
#define	get_errno JEMALLOC_N(get_errno)
#define	hash JEMALLOC_N(hash)
#define	hash_fmix_32 JEMALLOC_N(hash_fmix_32)
#define	hash_fmix_64 JEMALLOC_N(hash_fmix_64)
#define	hash_get_block_32 JEMALLOC_N(hash_get_block_32)
#define	hash_get_block_64 JEMALLOC_N(hash_get_block_64)
#define	hash_rotl_32 JEMALLOC_N(hash_rotl_32)
#define	hash_rotl_64 JEMALLOC_N(hash_rotl_64)
#define	hash_x64_128 JEMALLOC_N(hash_x64_128)
#define	hash_x86_128 JEMALLOC_N(hash_x86_128)
#define	hash_x86_32 JEMALLOC_N(hash_x86_32)
#define	iaalloc JEMALLOC_N(iaalloc)
#define	ialloc JEMALLOC_N(ialloc)
#define	iallocztm JEMALLOC_N(iallocztm)
#define	iarena_cleanup JEMALLOC_N(iarena_cleanup)
#define	idalloc JEMALLOC_N(idalloc)
#define	idalloctm JEMALLOC_N(idalloctm)
#define	iealloc JEMALLOC_N(iealloc)
#define	index2size JEMALLOC_N(index2size)
#define	index2size_compute JEMALLOC_N(index2size_compute)
#define	index2size_lookup JEMALLOC_N(index2size_lookup)
#define	index2size_tab JEMALLOC_N(index2size_tab)
#define	ipalloc JEMALLOC_N(ipalloc)
#define	ipalloct JEMALLOC_N(ipalloct)
#define	ipallocztm JEMALLOC_N(ipallocztm)
#define	iralloc JEMALLOC_N(iralloc)
#define	iralloct JEMALLOC_N(iralloct)
#define	iralloct_realign JEMALLOC_N(iralloct_realign)
#define	isalloc JEMALLOC_N(isalloc)
#define	isdalloct JEMALLOC_N(isdalloct)
#define	isthreaded JEMALLOC_N(isthreaded)
#define	ivsalloc JEMALLOC_N(ivsalloc)
#define	ixalloc JEMALLOC_N(ixalloc)
#define	jemalloc_postfork_child JEMALLOC_N(jemalloc_postfork_child)
#define	jemalloc_postfork_parent JEMALLOC_N(jemalloc_postfork_parent)
#define	jemalloc_prefork JEMALLOC_N(jemalloc_prefork)
#define	large_dalloc JEMALLOC_N(large_dalloc)
#define	large_dalloc_junk JEMALLOC_N(large_dalloc_junk)
#define	large_dalloc_junked_locked JEMALLOC_N(large_dalloc_junked_locked)
#define	large_dalloc_maybe_junk JEMALLOC_N(large_dalloc_maybe_junk)
#define	large_malloc JEMALLOC_N(large_malloc)
#define	large_palloc JEMALLOC_N(large_palloc)
#define	large_prof_tctx_get JEMALLOC_N(large_prof_tctx_get)
#define	large_prof_tctx_reset JEMALLOC_N(large_prof_tctx_reset)
#define	large_prof_tctx_set JEMALLOC_N(large_prof_tctx_set)
#define	large_ralloc JEMALLOC_N(large_ralloc)
#define	large_ralloc_no_move JEMALLOC_N(large_ralloc_no_move)
#define	large_salloc JEMALLOC_N(large_salloc)
#define	lg_floor JEMALLOC_N(lg_floor)
#define	lg_prof_sample JEMALLOC_N(lg_prof_sample)
#define	malloc_cprintf JEMALLOC_N(malloc_cprintf)
#define	malloc_mutex_assert_not_owner JEMALLOC_N(malloc_mutex_assert_not_owner)
#define	malloc_mutex_assert_owner JEMALLOC_N(malloc_mutex_assert_owner)
#define	malloc_mutex_boot JEMALLOC_N(malloc_mutex_boot)
#define	malloc_mutex_init JEMALLOC_N(malloc_mutex_init)
#define	malloc_mutex_lock JEMALLOC_N(malloc_mutex_lock)
#define	malloc_mutex_postfork_child JEMALLOC_N(malloc_mutex_postfork_child)
#define	malloc_mutex_postfork_parent JEMALLOC_N(malloc_mutex_postfork_parent)
#define	malloc_mutex_prefork JEMALLOC_N(malloc_mutex_prefork)
#define	malloc_mutex_unlock JEMALLOC_N(malloc_mutex_unlock)
#define	malloc_printf JEMALLOC_N(malloc_printf)
#define	malloc_snprintf JEMALLOC_N(malloc_snprintf)
#define	malloc_strtoumax JEMALLOC_N(malloc_strtoumax)
#define	malloc_tsd_boot0 JEMALLOC_N(malloc_tsd_boot0)
#define	malloc_tsd_boot1 JEMALLOC_N(malloc_tsd_boot1)
#define	malloc_tsd_cleanup_register JEMALLOC_N(malloc_tsd_cleanup_register)
#define	malloc_tsd_dalloc JEMALLOC_N(malloc_tsd_dalloc)
#define	malloc_tsd_malloc JEMALLOC_N(malloc_tsd_malloc)
#define	malloc_tsd_no_cleanup JEMALLOC_N(malloc_tsd_no_cleanup)
#define	malloc_vcprintf JEMALLOC_N(malloc_vcprintf)
#define	malloc_vsnprintf JEMALLOC_N(malloc_vsnprintf)
#define	malloc_write JEMALLOC_N(malloc_write)
#define	mb_write JEMALLOC_N(mb_write)
#define	narenas_auto JEMALLOC_N(narenas_auto)
#define	narenas_total_get JEMALLOC_N(narenas_total_get)
#define	ncpus JEMALLOC_N(ncpus)
#define	nhbins JEMALLOC_N(nhbins)
#define	nstime_add JEMALLOC_N(nstime_add)
#define	nstime_compare JEMALLOC_N(nstime_compare)
#define	nstime_copy JEMALLOC_N(nstime_copy)
#define	nstime_divide JEMALLOC_N(nstime_divide)
#define	nstime_idivide JEMALLOC_N(nstime_idivide)
#define	nstime_imultiply JEMALLOC_N(nstime_imultiply)
#define	nstime_init JEMALLOC_N(nstime_init)
#define	nstime_init2 JEMALLOC_N(nstime_init2)
#define	nstime_monotonic JEMALLOC_N(nstime_monotonic)
#define	nstime_ns JEMALLOC_N(nstime_ns)
#define	nstime_nsec JEMALLOC_N(nstime_nsec)
#define	nstime_sec JEMALLOC_N(nstime_sec)
#define	nstime_subtract JEMALLOC_N(nstime_subtract)
#define	nstime_update JEMALLOC_N(nstime_update)
#define	opt_abort JEMALLOC_N(opt_abort)
#define	opt_decay_time JEMALLOC_N(opt_decay_time)
#define	opt_dss JEMALLOC_N(opt_dss)
#define	opt_junk JEMALLOC_N(opt_junk)
#define	opt_junk_alloc JEMALLOC_N(opt_junk_alloc)
#define	opt_junk_free JEMALLOC_N(opt_junk_free)
#define	opt_lg_prof_interval JEMALLOC_N(opt_lg_prof_interval)
#define	opt_lg_prof_sample JEMALLOC_N(opt_lg_prof_sample)
#define	opt_lg_tcache_max JEMALLOC_N(opt_lg_tcache_max)
#define	opt_narenas JEMALLOC_N(opt_narenas)
#define	opt_prof JEMALLOC_N(opt_prof)
#define	opt_prof_accum JEMALLOC_N(opt_prof_accum)
#define	opt_prof_active JEMALLOC_N(opt_prof_active)
#define	opt_prof_final JEMALLOC_N(opt_prof_final)
#define	opt_prof_gdump JEMALLOC_N(opt_prof_gdump)
#define	opt_prof_leak JEMALLOC_N(opt_prof_leak)
#define	opt_prof_prefix JEMALLOC_N(opt_prof_prefix)
#define	opt_prof_thread_active_init JEMALLOC_N(opt_prof_thread_active_init)
#define	opt_stats_print JEMALLOC_N(opt_stats_print)
#define	opt_tcache JEMALLOC_N(opt_tcache)
#define	opt_utrace JEMALLOC_N(opt_utrace)
#define	opt_xmalloc JEMALLOC_N(opt_xmalloc)
#define	opt_zero JEMALLOC_N(opt_zero)
#define	pages_boot JEMALLOC_N(pages_boot)
#define	pages_commit JEMALLOC_N(pages_commit)
#define	pages_decommit JEMALLOC_N(pages_decommit)
#define	pages_huge JEMALLOC_N(pages_huge)
#define	pages_map JEMALLOC_N(pages_map)
#define	pages_nohuge JEMALLOC_N(pages_nohuge)
#define	pages_purge_forced JEMALLOC_N(pages_purge_forced)
#define	pages_purge_lazy JEMALLOC_N(pages_purge_lazy)
#define	pages_trim JEMALLOC_N(pages_trim)
#define	pages_unmap JEMALLOC_N(pages_unmap)
#define	pind2sz JEMALLOC_N(pind2sz)
#define	pind2sz_compute JEMALLOC_N(pind2sz_compute)
#define	pind2sz_lookup JEMALLOC_N(pind2sz_lookup)
#define	pind2sz_tab JEMALLOC_N(pind2sz_tab)
#define	pow2_ceil_u32 JEMALLOC_N(pow2_ceil_u32)
#define	pow2_ceil_u64 JEMALLOC_N(pow2_ceil_u64)
#define	pow2_ceil_zu JEMALLOC_N(pow2_ceil_zu)
#define	prng_lg_range_u32 JEMALLOC_N(prng_lg_range_u32)
#define	prng_lg_range_u64 JEMALLOC_N(prng_lg_range_u64)
#define	prng_lg_range_zu JEMALLOC_N(prng_lg_range_zu)
#define	prng_range_u32 JEMALLOC_N(prng_range_u32)
#define	prng_range_u64 JEMALLOC_N(prng_range_u64)
#define	prng_range_zu JEMALLOC_N(prng_range_zu)
#define	prng_state_next_u32 JEMALLOC_N(prng_state_next_u32)
#define	prng_state_next_u64 JEMALLOC_N(prng_state_next_u64)
#define	prng_state_next_zu JEMALLOC_N(prng_state_next_zu)
#define	prof_active JEMALLOC_N(prof_active)
#define	prof_active_get JEMALLOC_N(prof_active_get)
#define	prof_active_get_unlocked JEMALLOC_N(prof_active_get_unlocked)
#define	prof_active_set JEMALLOC_N(prof_active_set)
#define	prof_alloc_prep JEMALLOC_N(prof_alloc_prep)
#define	prof_alloc_rollback JEMALLOC_N(prof_alloc_rollback)
#define	prof_backtrace JEMALLOC_N(prof_backtrace)
#define	prof_boot0 JEMALLOC_N(prof_boot0)
#define	prof_boot1 JEMALLOC_N(prof_boot1)
#define	prof_boot2 JEMALLOC_N(prof_boot2)
#define	prof_bt_count JEMALLOC_N(prof_bt_count)
#define	prof_dump_header JEMALLOC_N(prof_dump_header)
#define	prof_dump_open JEMALLOC_N(prof_dump_open)
#define	prof_free JEMALLOC_N(prof_free)
#define	prof_free_sampled_object JEMALLOC_N(prof_free_sampled_object)
#define	prof_gdump JEMALLOC_N(prof_gdump)
#define	prof_gdump_get JEMALLOC_N(prof_gdump_get)
#define	prof_gdump_get_unlocked JEMALLOC_N(prof_gdump_get_unlocked)
#define	prof_gdump_set JEMALLOC_N(prof_gdump_set)
#define	prof_gdump_val JEMALLOC_N(prof_gdump_val)
#define	prof_idump JEMALLOC_N(prof_idump)
#define	prof_interval JEMALLOC_N(prof_interval)
#define	prof_lookup JEMALLOC_N(prof_lookup)
#define	prof_malloc JEMALLOC_N(prof_malloc)
#define	prof_malloc_sample_object JEMALLOC_N(prof_malloc_sample_object)
#define	prof_mdump JEMALLOC_N(prof_mdump)
#define	prof_postfork_child JEMALLOC_N(prof_postfork_child)
#define	prof_postfork_parent JEMALLOC_N(prof_postfork_parent)
#define	prof_prefork0 JEMALLOC_N(prof_prefork0)
#define	prof_prefork1 JEMALLOC_N(prof_prefork1)
#define	prof_realloc JEMALLOC_N(prof_realloc)
#define	prof_reset JEMALLOC_N(prof_reset)
#define	prof_sample_accum_update JEMALLOC_N(prof_sample_accum_update)
#define	prof_sample_threshold_update JEMALLOC_N(prof_sample_threshold_update)
#define	prof_tctx_get JEMALLOC_N(prof_tctx_get)
#define	prof_tctx_reset JEMALLOC_N(prof_tctx_reset)
#define	prof_tctx_set JEMALLOC_N(prof_tctx_set)
#define	prof_tdata_cleanup JEMALLOC_N(prof_tdata_cleanup)
#define	prof_tdata_count JEMALLOC_N(prof_tdata_count)
#define	prof_tdata_get JEMALLOC_N(prof_tdata_get)
#define	prof_tdata_init JEMALLOC_N(prof_tdata_init)
#define	prof_tdata_reinit JEMALLOC_N(prof_tdata_reinit)
#define	prof_thread_active_get JEMALLOC_N(prof_thread_active_get)
#define	prof_thread_active_init_get JEMALLOC_N(prof_thread_active_init_get)
#define	prof_thread_active_init_set JEMALLOC_N(prof_thread_active_init_set)
#define	prof_thread_active_set JEMALLOC_N(prof_thread_active_set)
#define	prof_thread_name_get JEMALLOC_N(prof_thread_name_get)
#define	prof_thread_name_set JEMALLOC_N(prof_thread_name_set)
#define	psz2ind JEMALLOC_N(psz2ind)
#define	psz2u JEMALLOC_N(psz2u)
#define	rtree_child_read JEMALLOC_N(rtree_child_read)
#define	rtree_child_read_hard JEMALLOC_N(rtree_child_read_hard)
#define	rtree_child_tryread JEMALLOC_N(rtree_child_tryread)
#define	rtree_clear JEMALLOC_N(rtree_clear)
#define	rtree_ctx_start_level JEMALLOC_N(rtree_ctx_start_level)
#define	rtree_delete JEMALLOC_N(rtree_delete)
#define	rtree_elm_acquire JEMALLOC_N(rtree_elm_acquire)
#define	rtree_elm_lookup JEMALLOC_N(rtree_elm_lookup)
#define	rtree_elm_read JEMALLOC_N(rtree_elm_read)
#define	rtree_elm_read_acquired JEMALLOC_N(rtree_elm_read_acquired)
#define	rtree_elm_release JEMALLOC_N(rtree_elm_release)
#define	rtree_elm_witness_access JEMALLOC_N(rtree_elm_witness_access)
#define	rtree_elm_witness_acquire JEMALLOC_N(rtree_elm_witness_acquire)
#define	rtree_elm_witness_release JEMALLOC_N(rtree_elm_witness_release)
#define	rtree_elm_write JEMALLOC_N(rtree_elm_write)
#define	rtree_elm_write_acquired JEMALLOC_N(rtree_elm_write_acquired)
#define	rtree_new JEMALLOC_N(rtree_new)
#define	rtree_node_alloc JEMALLOC_N(rtree_node_alloc)
#define	rtree_node_dalloc JEMALLOC_N(rtree_node_dalloc)
#define	rtree_node_valid JEMALLOC_N(rtree_node_valid)
#define	rtree_read JEMALLOC_N(rtree_read)
#define	rtree_start_level JEMALLOC_N(rtree_start_level)
#define	rtree_subkey JEMALLOC_N(rtree_subkey)
#define	rtree_subtree_read JEMALLOC_N(rtree_subtree_read)
#define	rtree_subtree_read_hard JEMALLOC_N(rtree_subtree_read_hard)
#define	rtree_subtree_tryread JEMALLOC_N(rtree_subtree_tryread)
#define	rtree_write JEMALLOC_N(rtree_write)
#define	s2u JEMALLOC_N(s2u)
#define	s2u_compute JEMALLOC_N(s2u_compute)
#define	s2u_lookup JEMALLOC_N(s2u_lookup)
#define	sa2u JEMALLOC_N(sa2u)
#define	set_errno JEMALLOC_N(set_errno)
#define	size2index JEMALLOC_N(size2index)
#define	size2index_compute JEMALLOC_N(size2index_compute)
#define	size2index_lookup JEMALLOC_N(size2index_lookup)
#define	size2index_tab JEMALLOC_N(size2index_tab)
#define	spin_adaptive JEMALLOC_N(spin_adaptive)
#define	spin_init JEMALLOC_N(spin_init)
#define	stats_print JEMALLOC_N(stats_print)
#define	tcache_alloc_easy JEMALLOC_N(tcache_alloc_easy)
#define	tcache_alloc_large JEMALLOC_N(tcache_alloc_large)
#define	tcache_alloc_small JEMALLOC_N(tcache_alloc_small)
#define	tcache_alloc_small_hard JEMALLOC_N(tcache_alloc_small_hard)
#define	tcache_arena_reassociate JEMALLOC_N(tcache_arena_reassociate)
#define	tcache_bin_flush_large JEMALLOC_N(tcache_bin_flush_large)
#define	tcache_bin_flush_small JEMALLOC_N(tcache_bin_flush_small)
#define	tcache_bin_info JEMALLOC_N(tcache_bin_info)
#define	tcache_boot JEMALLOC_N(tcache_boot)
#define	tcache_cleanup JEMALLOC_N(tcache_cleanup)
#define	tcache_create JEMALLOC_N(tcache_create)
#define	tcache_dalloc_large JEMALLOC_N(tcache_dalloc_large)
#define	tcache_dalloc_small JEMALLOC_N(tcache_dalloc_small)
#define	tcache_enabled_get JEMALLOC_N(tcache_enabled_get)
#define	tcache_enabled_set JEMALLOC_N(tcache_enabled_set)
#define	tcache_event JEMALLOC_N(tcache_event)
#define	tcache_event_hard JEMALLOC_N(tcache_event_hard)
#define	tcache_flush JEMALLOC_N(tcache_flush)
#define	tcache_get JEMALLOC_N(tcache_get)
#define	tcache_get_hard JEMALLOC_N(tcache_get_hard)
#define	tcache_maxclass JEMALLOC_N(tcache_maxclass)
#define	tcache_salloc JEMALLOC_N(tcache_salloc)
#define	tcache_stats_merge JEMALLOC_N(tcache_stats_merge)
#define	tcaches JEMALLOC_N(tcaches)
#define	tcaches_create JEMALLOC_N(tcaches_create)
#define	tcaches_destroy JEMALLOC_N(tcaches_destroy)
#define	tcaches_flush JEMALLOC_N(tcaches_flush)
#define	tcaches_get JEMALLOC_N(tcaches_get)
#define	ticker_copy JEMALLOC_N(ticker_copy)
#define	ticker_init JEMALLOC_N(ticker_init)
#define	ticker_read JEMALLOC_N(ticker_read)
#define	ticker_tick JEMALLOC_N(ticker_tick)
#define	ticker_ticks JEMALLOC_N(ticker_ticks)
#define	tsd_arena_get JEMALLOC_N(tsd_arena_get)
#define	tsd_arena_set JEMALLOC_N(tsd_arena_set)
#define	tsd_arenap_get JEMALLOC_N(tsd_arenap_get)
#define	tsd_arenas_tdata_bypass_get JEMALLOC_N(tsd_arenas_tdata_bypass_get)
#define	tsd_arenas_tdata_bypass_set JEMALLOC_N(tsd_arenas_tdata_bypass_set)
#define	tsd_arenas_tdata_bypassp_get JEMALLOC_N(tsd_arenas_tdata_bypassp_get)
#define	tsd_arenas_tdata_get JEMALLOC_N(tsd_arenas_tdata_get)
#define	tsd_arenas_tdata_set JEMALLOC_N(tsd_arenas_tdata_set)
#define	tsd_arenas_tdatap_get JEMALLOC_N(tsd_arenas_tdatap_get)
#define	tsd_boot JEMALLOC_N(tsd_boot)
#define	tsd_boot0 JEMALLOC_N(tsd_boot0)
#define	tsd_boot1 JEMALLOC_N(tsd_boot1)
#define	tsd_booted JEMALLOC_N(tsd_booted)
#define	tsd_booted_get JEMALLOC_N(tsd_booted_get)
#define	tsd_cleanup JEMALLOC_N(tsd_cleanup)
#define	tsd_cleanup_wrapper JEMALLOC_N(tsd_cleanup_wrapper)
#define	tsd_fetch JEMALLOC_N(tsd_fetch)
#define	tsd_fetch_impl JEMALLOC_N(tsd_fetch_impl)
#define	tsd_get JEMALLOC_N(tsd_get)
#define	tsd_get_allocates JEMALLOC_N(tsd_get_allocates)
#define	tsd_iarena_get JEMALLOC_N(tsd_iarena_get)
#define	tsd_iarena_set JEMALLOC_N(tsd_iarena_set)
#define	tsd_iarenap_get JEMALLOC_N(tsd_iarenap_get)
#define	tsd_initialized JEMALLOC_N(tsd_initialized)
#define	tsd_init_check_recursion JEMALLOC_N(tsd_init_check_recursion)
#define	tsd_init_finish JEMALLOC_N(tsd_init_finish)
#define	tsd_init_head JEMALLOC_N(tsd_init_head)
#define	tsd_narenas_tdata_get JEMALLOC_N(tsd_narenas_tdata_get)
#define	tsd_narenas_tdata_set JEMALLOC_N(tsd_narenas_tdata_set)
#define	tsd_narenas_tdatap_get JEMALLOC_N(tsd_narenas_tdatap_get)
#define	tsd_wrapper_get JEMALLOC_N(tsd_wrapper_get)
#define	tsd_wrapper_set JEMALLOC_N(tsd_wrapper_set)
#define	tsd_nominal JEMALLOC_N(tsd_nominal)
#define	tsd_prof_tdata_get JEMALLOC_N(tsd_prof_tdata_get)
#define	tsd_prof_tdata_set JEMALLOC_N(tsd_prof_tdata_set)
#define	tsd_prof_tdatap_get JEMALLOC_N(tsd_prof_tdatap_get)
#define	tsd_rtree_ctx_get JEMALLOC_N(tsd_rtree_ctx_get)
#define	tsd_rtree_ctx_set JEMALLOC_N(tsd_rtree_ctx_set)
#define	tsd_rtree_ctxp_get JEMALLOC_N(tsd_rtree_ctxp_get)
#define	tsd_rtree_elm_witnesses_get JEMALLOC_N(tsd_rtree_elm_witnesses_get)
#define	tsd_rtree_elm_witnesses_set JEMALLOC_N(tsd_rtree_elm_witnesses_set)
#define	tsd_rtree_elm_witnessesp_get JEMALLOC_N(tsd_rtree_elm_witnessesp_get)
#define	tsd_set JEMALLOC_N(tsd_set)
#define	tsd_tcache_enabled_get JEMALLOC_N(tsd_tcache_enabled_get)
#define	tsd_tcache_enabled_set JEMALLOC_N(tsd_tcache_enabled_set)
#define	tsd_tcache_enabledp_get JEMALLOC_N(tsd_tcache_enabledp_get)
#define	tsd_tcache_get JEMALLOC_N(tsd_tcache_get)
#define	tsd_tcache_set JEMALLOC_N(tsd_tcache_set)
#define	tsd_tcachep_get JEMALLOC_N(tsd_tcachep_get)
#define	tsd_thread_allocated_get JEMALLOC_N(tsd_thread_allocated_get)
#define	tsd_thread_allocated_set JEMALLOC_N(tsd_thread_allocated_set)
#define	tsd_thread_allocatedp_get JEMALLOC_N(tsd_thread_allocatedp_get)
#define	tsd_thread_deallocated_get JEMALLOC_N(tsd_thread_deallocated_get)
#define	tsd_thread_deallocated_set JEMALLOC_N(tsd_thread_deallocated_set)
#define	tsd_thread_deallocatedp_get JEMALLOC_N(tsd_thread_deallocatedp_get)
#define	tsd_tls JEMALLOC_N(tsd_tls)
#define	tsd_tsd JEMALLOC_N(tsd_tsd)
#define	tsd_tsdn JEMALLOC_N(tsd_tsdn)
#define	tsd_witness_fork_get JEMALLOC_N(tsd_witness_fork_get)
#define	tsd_witness_fork_set JEMALLOC_N(tsd_witness_fork_set)
#define	tsd_witness_forkp_get JEMALLOC_N(tsd_witness_forkp_get)
#define	tsd_witnesses_get JEMALLOC_N(tsd_witnesses_get)
#define	tsd_witnesses_set JEMALLOC_N(tsd_witnesses_set)
#define	tsd_witnessesp_get JEMALLOC_N(tsd_witnessesp_get)
#define	tsdn_fetch JEMALLOC_N(tsdn_fetch)
#define	tsdn_null JEMALLOC_N(tsdn_null)
#define	tsdn_rtree_ctx JEMALLOC_N(tsdn_rtree_ctx)
#define	tsdn_tsd JEMALLOC_N(tsdn_tsd)
#define	witness_assert_lockless JEMALLOC_N(witness_assert_lockless)
#define	witness_assert_not_owner JEMALLOC_N(witness_assert_not_owner)
#define	witness_assert_owner JEMALLOC_N(witness_assert_owner)
#define	witness_init JEMALLOC_N(witness_init)
#define	witness_lock JEMALLOC_N(witness_lock)
#define	witness_lock_error JEMALLOC_N(witness_lock_error)
#define	witness_lockless_error JEMALLOC_N(witness_lockless_error)
#define	witness_not_owner_error JEMALLOC_N(witness_not_owner_error)
#define	witness_owner JEMALLOC_N(witness_owner)
#define	witness_owner_error JEMALLOC_N(witness_owner_error)
#define	witness_postfork_child JEMALLOC_N(witness_postfork_child)
#define	witness_postfork_parent JEMALLOC_N(witness_postfork_parent)
#define	witness_prefork JEMALLOC_N(witness_prefork)
#define	witness_unlock JEMALLOC_N(witness_unlock)
#define	witnesses_cleanup JEMALLOC_N(witnesses_cleanup)
#define	zone_register JEMALLOC_N(zone_register)
