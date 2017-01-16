#define	JEMALLOC_CTL_C_
#include "jemalloc/internal/jemalloc_internal.h"

/******************************************************************************/
/* Data. */

/*
 * ctl_mtx protects the following:
 * - ctl_stats->*
 */
static malloc_mutex_t	ctl_mtx;
static bool		ctl_initialized;
static ctl_stats_t	*ctl_stats;

/******************************************************************************/
/* Helpers for named and indexed nodes. */

JEMALLOC_INLINE_C const ctl_named_node_t *
ctl_named_node(const ctl_node_t *node)
{
	return ((node->named) ? (const ctl_named_node_t *)node : NULL);
}

JEMALLOC_INLINE_C const ctl_named_node_t *
ctl_named_children(const ctl_named_node_t *node, size_t index)
{
	const ctl_named_node_t *children = ctl_named_node(node->children);

	return (children ? &children[index] : NULL);
}

JEMALLOC_INLINE_C const ctl_indexed_node_t *
ctl_indexed_node(const ctl_node_t *node)
{
	return (!node->named ? (const ctl_indexed_node_t *)node : NULL);
}

/******************************************************************************/
/* Function prototypes for non-inline static functions. */

#define	CTL_PROTO(n)							\
static int	n##_ctl(tsd_t *tsd, const size_t *mib, size_t miblen,	\
    void *oldp, size_t *oldlenp, void *newp, size_t newlen);

#define	INDEX_PROTO(n)							\
static const ctl_named_node_t	*n##_index(tsdn_t *tsdn,		\
    const size_t *mib, size_t miblen, size_t i);

CTL_PROTO(version)
CTL_PROTO(epoch)
CTL_PROTO(thread_tcache_enabled)
CTL_PROTO(thread_tcache_flush)
CTL_PROTO(thread_prof_name)
CTL_PROTO(thread_prof_active)
CTL_PROTO(thread_arena)
CTL_PROTO(thread_allocated)
CTL_PROTO(thread_allocatedp)
CTL_PROTO(thread_deallocated)
CTL_PROTO(thread_deallocatedp)
CTL_PROTO(config_cache_oblivious)
CTL_PROTO(config_debug)
CTL_PROTO(config_fill)
CTL_PROTO(config_lazy_lock)
CTL_PROTO(config_malloc_conf)
CTL_PROTO(config_munmap)
CTL_PROTO(config_prof)
CTL_PROTO(config_prof_libgcc)
CTL_PROTO(config_prof_libunwind)
CTL_PROTO(config_stats)
CTL_PROTO(config_tcache)
CTL_PROTO(config_tls)
CTL_PROTO(config_utrace)
CTL_PROTO(config_xmalloc)
CTL_PROTO(opt_abort)
CTL_PROTO(opt_dss)
CTL_PROTO(opt_narenas)
CTL_PROTO(opt_decay_time)
CTL_PROTO(opt_stats_print)
CTL_PROTO(opt_junk)
CTL_PROTO(opt_zero)
CTL_PROTO(opt_utrace)
CTL_PROTO(opt_xmalloc)
CTL_PROTO(opt_tcache)
CTL_PROTO(opt_lg_tcache_max)
CTL_PROTO(opt_prof)
CTL_PROTO(opt_prof_prefix)
CTL_PROTO(opt_prof_active)
CTL_PROTO(opt_prof_thread_active_init)
CTL_PROTO(opt_lg_prof_sample)
CTL_PROTO(opt_lg_prof_interval)
CTL_PROTO(opt_prof_gdump)
CTL_PROTO(opt_prof_final)
CTL_PROTO(opt_prof_leak)
CTL_PROTO(opt_prof_accum)
CTL_PROTO(tcache_create)
CTL_PROTO(tcache_flush)
CTL_PROTO(tcache_destroy)
CTL_PROTO(arena_i_initialized)
CTL_PROTO(arena_i_purge)
CTL_PROTO(arena_i_decay)
CTL_PROTO(arena_i_reset)
CTL_PROTO(arena_i_destroy)
CTL_PROTO(arena_i_dss)
CTL_PROTO(arena_i_decay_time)
CTL_PROTO(arena_i_extent_hooks)
INDEX_PROTO(arena_i)
CTL_PROTO(arenas_bin_i_size)
CTL_PROTO(arenas_bin_i_nregs)
CTL_PROTO(arenas_bin_i_slab_size)
INDEX_PROTO(arenas_bin_i)
CTL_PROTO(arenas_lextent_i_size)
INDEX_PROTO(arenas_lextent_i)
CTL_PROTO(arenas_narenas)
CTL_PROTO(arenas_decay_time)
CTL_PROTO(arenas_quantum)
CTL_PROTO(arenas_page)
CTL_PROTO(arenas_tcache_max)
CTL_PROTO(arenas_nbins)
CTL_PROTO(arenas_nhbins)
CTL_PROTO(arenas_nlextents)
CTL_PROTO(arenas_create)
CTL_PROTO(prof_thread_active_init)
CTL_PROTO(prof_active)
CTL_PROTO(prof_dump)
CTL_PROTO(prof_gdump)
CTL_PROTO(prof_reset)
CTL_PROTO(prof_interval)
CTL_PROTO(lg_prof_sample)
CTL_PROTO(stats_arenas_i_small_allocated)
CTL_PROTO(stats_arenas_i_small_nmalloc)
CTL_PROTO(stats_arenas_i_small_ndalloc)
CTL_PROTO(stats_arenas_i_small_nrequests)
CTL_PROTO(stats_arenas_i_large_allocated)
CTL_PROTO(stats_arenas_i_large_nmalloc)
CTL_PROTO(stats_arenas_i_large_ndalloc)
CTL_PROTO(stats_arenas_i_large_nrequests)
CTL_PROTO(stats_arenas_i_bins_j_nmalloc)
CTL_PROTO(stats_arenas_i_bins_j_ndalloc)
CTL_PROTO(stats_arenas_i_bins_j_nrequests)
CTL_PROTO(stats_arenas_i_bins_j_curregs)
CTL_PROTO(stats_arenas_i_bins_j_nfills)
CTL_PROTO(stats_arenas_i_bins_j_nflushes)
CTL_PROTO(stats_arenas_i_bins_j_nslabs)
CTL_PROTO(stats_arenas_i_bins_j_nreslabs)
CTL_PROTO(stats_arenas_i_bins_j_curslabs)
INDEX_PROTO(stats_arenas_i_bins_j)
CTL_PROTO(stats_arenas_i_lextents_j_nmalloc)
CTL_PROTO(stats_arenas_i_lextents_j_ndalloc)
CTL_PROTO(stats_arenas_i_lextents_j_nrequests)
CTL_PROTO(stats_arenas_i_lextents_j_curlextents)
INDEX_PROTO(stats_arenas_i_lextents_j)
CTL_PROTO(stats_arenas_i_nthreads)
CTL_PROTO(stats_arenas_i_dss)
CTL_PROTO(stats_arenas_i_decay_time)
CTL_PROTO(stats_arenas_i_pactive)
CTL_PROTO(stats_arenas_i_pdirty)
CTL_PROTO(stats_arenas_i_mapped)
CTL_PROTO(stats_arenas_i_retained)
CTL_PROTO(stats_arenas_i_npurge)
CTL_PROTO(stats_arenas_i_nmadvise)
CTL_PROTO(stats_arenas_i_purged)
CTL_PROTO(stats_arenas_i_base)
CTL_PROTO(stats_arenas_i_internal)
CTL_PROTO(stats_arenas_i_resident)
INDEX_PROTO(stats_arenas_i)
CTL_PROTO(stats_allocated)
CTL_PROTO(stats_active)
CTL_PROTO(stats_metadata)
CTL_PROTO(stats_resident)
CTL_PROTO(stats_mapped)
CTL_PROTO(stats_retained)

/******************************************************************************/
/* mallctl tree. */

/* Maximum tree depth. */
#define	CTL_MAX_DEPTH	6

#define	NAME(n)	{true},	n
#define	CHILD(t, c)							\
	sizeof(c##_node) / sizeof(ctl_##t##_node_t),			\
	(ctl_node_t *)c##_node,						\
	NULL
#define	CTL(c)	0, NULL, c##_ctl

/*
 * Only handles internal indexed nodes, since there are currently no external
 * ones.
 */
#define	INDEX(i)	{false},	i##_index

static const ctl_named_node_t	thread_tcache_node[] = {
	{NAME("enabled"),	CTL(thread_tcache_enabled)},
	{NAME("flush"),		CTL(thread_tcache_flush)}
};

static const ctl_named_node_t	thread_prof_node[] = {
	{NAME("name"),		CTL(thread_prof_name)},
	{NAME("active"),	CTL(thread_prof_active)}
};

static const ctl_named_node_t	thread_node[] = {
	{NAME("arena"),		CTL(thread_arena)},
	{NAME("allocated"),	CTL(thread_allocated)},
	{NAME("allocatedp"),	CTL(thread_allocatedp)},
	{NAME("deallocated"),	CTL(thread_deallocated)},
	{NAME("deallocatedp"),	CTL(thread_deallocatedp)},
	{NAME("tcache"),	CHILD(named, thread_tcache)},
	{NAME("prof"),		CHILD(named, thread_prof)}
};

static const ctl_named_node_t	config_node[] = {
	{NAME("cache_oblivious"), CTL(config_cache_oblivious)},
	{NAME("debug"),		CTL(config_debug)},
	{NAME("fill"),		CTL(config_fill)},
	{NAME("lazy_lock"),	CTL(config_lazy_lock)},
	{NAME("malloc_conf"),	CTL(config_malloc_conf)},
	{NAME("munmap"),	CTL(config_munmap)},
	{NAME("prof"),		CTL(config_prof)},
	{NAME("prof_libgcc"),	CTL(config_prof_libgcc)},
	{NAME("prof_libunwind"), CTL(config_prof_libunwind)},
	{NAME("stats"),		CTL(config_stats)},
	{NAME("tcache"),	CTL(config_tcache)},
	{NAME("tls"),		CTL(config_tls)},
	{NAME("utrace"),	CTL(config_utrace)},
	{NAME("xmalloc"),	CTL(config_xmalloc)}
};

static const ctl_named_node_t opt_node[] = {
	{NAME("abort"),		CTL(opt_abort)},
	{NAME("dss"),		CTL(opt_dss)},
	{NAME("narenas"),	CTL(opt_narenas)},
	{NAME("decay_time"),	CTL(opt_decay_time)},
	{NAME("stats_print"),	CTL(opt_stats_print)},
	{NAME("junk"),		CTL(opt_junk)},
	{NAME("zero"),		CTL(opt_zero)},
	{NAME("utrace"),	CTL(opt_utrace)},
	{NAME("xmalloc"),	CTL(opt_xmalloc)},
	{NAME("tcache"),	CTL(opt_tcache)},
	{NAME("lg_tcache_max"),	CTL(opt_lg_tcache_max)},
	{NAME("prof"),		CTL(opt_prof)},
	{NAME("prof_prefix"),	CTL(opt_prof_prefix)},
	{NAME("prof_active"),	CTL(opt_prof_active)},
	{NAME("prof_thread_active_init"), CTL(opt_prof_thread_active_init)},
	{NAME("lg_prof_sample"), CTL(opt_lg_prof_sample)},
	{NAME("lg_prof_interval"), CTL(opt_lg_prof_interval)},
	{NAME("prof_gdump"),	CTL(opt_prof_gdump)},
	{NAME("prof_final"),	CTL(opt_prof_final)},
	{NAME("prof_leak"),	CTL(opt_prof_leak)},
	{NAME("prof_accum"),	CTL(opt_prof_accum)}
};

static const ctl_named_node_t	tcache_node[] = {
	{NAME("create"),	CTL(tcache_create)},
	{NAME("flush"),		CTL(tcache_flush)},
	{NAME("destroy"),	CTL(tcache_destroy)}
};

static const ctl_named_node_t arena_i_node[] = {
	{NAME("initialized"),	CTL(arena_i_initialized)},
	{NAME("purge"),		CTL(arena_i_purge)},
	{NAME("decay"),		CTL(arena_i_decay)},
	{NAME("reset"),		CTL(arena_i_reset)},
	{NAME("destroy"),	CTL(arena_i_destroy)},
	{NAME("dss"),		CTL(arena_i_dss)},
	{NAME("decay_time"),	CTL(arena_i_decay_time)},
	{NAME("extent_hooks"),	CTL(arena_i_extent_hooks)}
};
static const ctl_named_node_t super_arena_i_node[] = {
	{NAME(""),		CHILD(named, arena_i)}
};

static const ctl_indexed_node_t arena_node[] = {
	{INDEX(arena_i)}
};

static const ctl_named_node_t arenas_bin_i_node[] = {
	{NAME("size"),		CTL(arenas_bin_i_size)},
	{NAME("nregs"),		CTL(arenas_bin_i_nregs)},
	{NAME("slab_size"),	CTL(arenas_bin_i_slab_size)}
};
static const ctl_named_node_t super_arenas_bin_i_node[] = {
	{NAME(""),		CHILD(named, arenas_bin_i)}
};

static const ctl_indexed_node_t arenas_bin_node[] = {
	{INDEX(arenas_bin_i)}
};

static const ctl_named_node_t arenas_lextent_i_node[] = {
	{NAME("size"),		CTL(arenas_lextent_i_size)}
};
static const ctl_named_node_t super_arenas_lextent_i_node[] = {
	{NAME(""),		CHILD(named, arenas_lextent_i)}
};

static const ctl_indexed_node_t arenas_lextent_node[] = {
	{INDEX(arenas_lextent_i)}
};

static const ctl_named_node_t arenas_node[] = {
	{NAME("narenas"),	CTL(arenas_narenas)},
	{NAME("decay_time"),	CTL(arenas_decay_time)},
	{NAME("quantum"),	CTL(arenas_quantum)},
	{NAME("page"),		CTL(arenas_page)},
	{NAME("tcache_max"),	CTL(arenas_tcache_max)},
	{NAME("nbins"),		CTL(arenas_nbins)},
	{NAME("nhbins"),	CTL(arenas_nhbins)},
	{NAME("bin"),		CHILD(indexed, arenas_bin)},
	{NAME("nlextents"),	CTL(arenas_nlextents)},
	{NAME("lextent"),	CHILD(indexed, arenas_lextent)},
	{NAME("create"),	CTL(arenas_create)}
};

static const ctl_named_node_t	prof_node[] = {
	{NAME("thread_active_init"), CTL(prof_thread_active_init)},
	{NAME("active"),	CTL(prof_active)},
	{NAME("dump"),		CTL(prof_dump)},
	{NAME("gdump"),		CTL(prof_gdump)},
	{NAME("reset"),		CTL(prof_reset)},
	{NAME("interval"),	CTL(prof_interval)},
	{NAME("lg_sample"),	CTL(lg_prof_sample)}
};

static const ctl_named_node_t stats_arenas_i_small_node[] = {
	{NAME("allocated"),	CTL(stats_arenas_i_small_allocated)},
	{NAME("nmalloc"),	CTL(stats_arenas_i_small_nmalloc)},
	{NAME("ndalloc"),	CTL(stats_arenas_i_small_ndalloc)},
	{NAME("nrequests"),	CTL(stats_arenas_i_small_nrequests)}
};

static const ctl_named_node_t stats_arenas_i_large_node[] = {
	{NAME("allocated"),	CTL(stats_arenas_i_large_allocated)},
	{NAME("nmalloc"),	CTL(stats_arenas_i_large_nmalloc)},
	{NAME("ndalloc"),	CTL(stats_arenas_i_large_ndalloc)},
	{NAME("nrequests"),	CTL(stats_arenas_i_large_nrequests)}
};

static const ctl_named_node_t stats_arenas_i_bins_j_node[] = {
	{NAME("nmalloc"),	CTL(stats_arenas_i_bins_j_nmalloc)},
	{NAME("ndalloc"),	CTL(stats_arenas_i_bins_j_ndalloc)},
	{NAME("nrequests"),	CTL(stats_arenas_i_bins_j_nrequests)},
	{NAME("curregs"),	CTL(stats_arenas_i_bins_j_curregs)},
	{NAME("nfills"),	CTL(stats_arenas_i_bins_j_nfills)},
	{NAME("nflushes"),	CTL(stats_arenas_i_bins_j_nflushes)},
	{NAME("nslabs"),	CTL(stats_arenas_i_bins_j_nslabs)},
	{NAME("nreslabs"),	CTL(stats_arenas_i_bins_j_nreslabs)},
	{NAME("curslabs"),	CTL(stats_arenas_i_bins_j_curslabs)}
};
static const ctl_named_node_t super_stats_arenas_i_bins_j_node[] = {
	{NAME(""),		CHILD(named, stats_arenas_i_bins_j)}
};

static const ctl_indexed_node_t stats_arenas_i_bins_node[] = {
	{INDEX(stats_arenas_i_bins_j)}
};

static const ctl_named_node_t stats_arenas_i_lextents_j_node[] = {
	{NAME("nmalloc"),	CTL(stats_arenas_i_lextents_j_nmalloc)},
	{NAME("ndalloc"),	CTL(stats_arenas_i_lextents_j_ndalloc)},
	{NAME("nrequests"),	CTL(stats_arenas_i_lextents_j_nrequests)},
	{NAME("curlextents"),	CTL(stats_arenas_i_lextents_j_curlextents)}
};
static const ctl_named_node_t super_stats_arenas_i_lextents_j_node[] = {
	{NAME(""),		CHILD(named, stats_arenas_i_lextents_j)}
};

static const ctl_indexed_node_t stats_arenas_i_lextents_node[] = {
	{INDEX(stats_arenas_i_lextents_j)}
};

static const ctl_named_node_t stats_arenas_i_node[] = {
	{NAME("nthreads"),	CTL(stats_arenas_i_nthreads)},
	{NAME("dss"),		CTL(stats_arenas_i_dss)},
	{NAME("decay_time"),	CTL(stats_arenas_i_decay_time)},
	{NAME("pactive"),	CTL(stats_arenas_i_pactive)},
	{NAME("pdirty"),	CTL(stats_arenas_i_pdirty)},
	{NAME("mapped"),	CTL(stats_arenas_i_mapped)},
	{NAME("retained"),	CTL(stats_arenas_i_retained)},
	{NAME("npurge"),	CTL(stats_arenas_i_npurge)},
	{NAME("nmadvise"),	CTL(stats_arenas_i_nmadvise)},
	{NAME("purged"),	CTL(stats_arenas_i_purged)},
	{NAME("base"),		CTL(stats_arenas_i_base)},
	{NAME("internal"),	CTL(stats_arenas_i_internal)},
	{NAME("resident"),	CTL(stats_arenas_i_resident)},
	{NAME("small"),		CHILD(named, stats_arenas_i_small)},
	{NAME("large"),		CHILD(named, stats_arenas_i_large)},
	{NAME("bins"),		CHILD(indexed, stats_arenas_i_bins)},
	{NAME("lextents"),	CHILD(indexed, stats_arenas_i_lextents)}
};
static const ctl_named_node_t super_stats_arenas_i_node[] = {
	{NAME(""),		CHILD(named, stats_arenas_i)}
};

static const ctl_indexed_node_t stats_arenas_node[] = {
	{INDEX(stats_arenas_i)}
};

static const ctl_named_node_t stats_node[] = {
	{NAME("allocated"),	CTL(stats_allocated)},
	{NAME("active"),	CTL(stats_active)},
	{NAME("metadata"),	CTL(stats_metadata)},
	{NAME("resident"),	CTL(stats_resident)},
	{NAME("mapped"),	CTL(stats_mapped)},
	{NAME("retained"),	CTL(stats_retained)},
	{NAME("arenas"),	CHILD(indexed, stats_arenas)}
};

static const ctl_named_node_t	root_node[] = {
	{NAME("version"),	CTL(version)},
	{NAME("epoch"),		CTL(epoch)},
	{NAME("thread"),	CHILD(named, thread)},
	{NAME("config"),	CHILD(named, config)},
	{NAME("opt"),		CHILD(named, opt)},
	{NAME("tcache"),	CHILD(named, tcache)},
	{NAME("arena"),		CHILD(indexed, arena)},
	{NAME("arenas"),	CHILD(named, arenas)},
	{NAME("prof"),		CHILD(named, prof)},
	{NAME("stats"),		CHILD(named, stats)}
};
static const ctl_named_node_t super_root_node[] = {
	{NAME(""),		CHILD(named, root)}
};

#undef NAME
#undef CHILD
#undef CTL
#undef INDEX

/******************************************************************************/

static unsigned
stats_arenas_i2a_impl(size_t i, bool compat, bool validate)
{
	unsigned a;

	cassert(config_stats);

	switch (i) {
	case MALLCTL_ARENAS_ALL:
		a = 0;
		break;
	case MALLCTL_ARENAS_DESTROYED:
		a = 1;
		break;
	default:
		if (compat && i == ctl_stats->narenas) {
			/*
			 * Provide deprecated backward compatibility for
			 * accessing the merged stats at index narenas rather
			 * than via MALLCTL_ARENAS_ALL.  This is scheduled for
			 * removal in 6.0.0.
			 */
			a = 0;
		} else if (validate && i >= ctl_stats->narenas)
			a = UINT_MAX;
		else {
			/*
			 * This function should never be called for an index
			 * more than one past the range of indices that have
			 * initialized stats.
			 */
			assert(i < ctl_stats->narenas || (!validate && i ==
			    ctl_stats->narenas));
			a = (unsigned)i + 2;
		}
		break;
	}

	return (a);
}

static unsigned
stats_arenas_i2a(size_t i)
{
	return (stats_arenas_i2a_impl(i, true, false));
}

static ctl_arena_stats_t *
stats_arenas_i_impl(tsdn_t *tsdn, size_t i, bool compat, bool init)
{
	ctl_arena_stats_t *ret;

	assert(!compat || !init);

	ret = ctl_stats->arenas[stats_arenas_i2a_impl(i, compat, false)];
	if (init && ret == NULL) {
		ret = (ctl_arena_stats_t *)base_alloc(tsdn, b0get(),
		    sizeof(ctl_arena_stats_t), QUANTUM);
		if (ret == NULL)
			return (NULL);
		ret->arena_ind = (unsigned)i;
		ctl_stats->arenas[stats_arenas_i2a_impl(i, compat, false)] =
		    ret;
	}

	assert(ret == NULL || stats_arenas_i2a(ret->arena_ind) ==
	    stats_arenas_i2a(i));
	return (ret);
}

static ctl_arena_stats_t *
stats_arenas_i(size_t i)
{
	ctl_arena_stats_t *ret = stats_arenas_i_impl(TSDN_NULL, i, true, false);
	assert(ret != NULL);
	return (ret);
}

static void
ctl_arena_clear(ctl_arena_stats_t *astats)
{
	astats->nthreads = 0;
	astats->dss = dss_prec_names[dss_prec_limit];
	astats->decay_time = -1;
	astats->pactive = 0;
	astats->pdirty = 0;
	if (config_stats) {
		memset(&astats->astats, 0, sizeof(arena_stats_t));
		astats->allocated_small = 0;
		astats->nmalloc_small = 0;
		astats->ndalloc_small = 0;
		astats->nrequests_small = 0;
		memset(astats->bstats, 0, NBINS * sizeof(malloc_bin_stats_t));
		memset(astats->lstats, 0, (NSIZES - NBINS) *
		    sizeof(malloc_large_stats_t));
	}
}

static void
ctl_arena_stats_amerge(tsdn_t *tsdn, ctl_arena_stats_t *cstats, arena_t *arena)
{
	unsigned i;

	if (config_stats) {
		arena_stats_merge(tsdn, arena, &cstats->nthreads, &cstats->dss,
		    &cstats->decay_time, &cstats->pactive, &cstats->pdirty,
		    &cstats->astats, cstats->bstats, cstats->lstats);

		for (i = 0; i < NBINS; i++) {
			cstats->allocated_small += cstats->bstats[i].curregs *
			    index2size(i);
			cstats->nmalloc_small += cstats->bstats[i].nmalloc;
			cstats->ndalloc_small += cstats->bstats[i].ndalloc;
			cstats->nrequests_small += cstats->bstats[i].nrequests;
		}
	} else {
		arena_basic_stats_merge(tsdn, arena, &cstats->nthreads,
		    &cstats->dss, &cstats->decay_time, &cstats->pactive,
		    &cstats->pdirty);
	}
}

static void
ctl_arena_stats_sdmerge(ctl_arena_stats_t *sdstats, ctl_arena_stats_t *astats,
    bool destroyed)
{
	unsigned i;

	if (!destroyed) {
		sdstats->nthreads += astats->nthreads;
		sdstats->pactive += astats->pactive;
		sdstats->pdirty += astats->pdirty;
	} else {
		assert(astats->nthreads == 0);
		assert(astats->pactive == 0);
		assert(astats->pdirty == 0);
	}

	if (config_stats) {
		if (!destroyed) {
			sdstats->astats.mapped += astats->astats.mapped;
			sdstats->astats.retained += astats->astats.retained;
		}
		sdstats->astats.npurge += astats->astats.npurge;
		sdstats->astats.nmadvise += astats->astats.nmadvise;
		sdstats->astats.purged += astats->astats.purged;

		if (!destroyed) {
			sdstats->astats.base += astats->astats.base;
			sdstats->astats.internal += astats->astats.internal;
			sdstats->astats.resident += astats->astats.resident;
		} else
			assert(astats->astats.internal == 0);

		if (!destroyed)
			sdstats->allocated_small += astats->allocated_small;
		else
			assert(astats->allocated_small == 0);
		sdstats->nmalloc_small += astats->nmalloc_small;
		sdstats->ndalloc_small += astats->ndalloc_small;
		sdstats->nrequests_small += astats->nrequests_small;

		if (!destroyed) {
			sdstats->astats.allocated_large +=
			    astats->astats.allocated_large;
		} else
			assert(astats->astats.allocated_large == 0);
		sdstats->astats.nmalloc_large += astats->astats.nmalloc_large;
		sdstats->astats.ndalloc_large += astats->astats.ndalloc_large;
		sdstats->astats.nrequests_large +=
		    astats->astats.nrequests_large;

		for (i = 0; i < NBINS; i++) {
			sdstats->bstats[i].nmalloc += astats->bstats[i].nmalloc;
			sdstats->bstats[i].ndalloc += astats->bstats[i].ndalloc;
			sdstats->bstats[i].nrequests +=
			    astats->bstats[i].nrequests;
			if (!destroyed) {
				sdstats->bstats[i].curregs +=
				    astats->bstats[i].curregs;
			} else
				assert(astats->bstats[i].curregs == 0);
			if (config_tcache) {
				sdstats->bstats[i].nfills +=
				    astats->bstats[i].nfills;
				sdstats->bstats[i].nflushes +=
				    astats->bstats[i].nflushes;
			}
			sdstats->bstats[i].nslabs += astats->bstats[i].nslabs;
			sdstats->bstats[i].reslabs += astats->bstats[i].reslabs;
			if (!destroyed) {
				sdstats->bstats[i].curslabs +=
				    astats->bstats[i].curslabs;
			} else
				assert(astats->bstats[i].curslabs == 0);
		}

		for (i = 0; i < NSIZES - NBINS; i++) {
			sdstats->lstats[i].nmalloc += astats->lstats[i].nmalloc;
			sdstats->lstats[i].ndalloc += astats->lstats[i].ndalloc;
			sdstats->lstats[i].nrequests +=
			    astats->lstats[i].nrequests;
			if (!destroyed) {
				sdstats->lstats[i].curlextents +=
				    astats->lstats[i].curlextents;
			} else
				assert(astats->lstats[i].curlextents == 0);
		}
	}
}

static void
ctl_arena_refresh(tsdn_t *tsdn, arena_t *arena, ctl_arena_stats_t *sdstats,
    unsigned i, bool destroyed)
{
	ctl_arena_stats_t *astats = stats_arenas_i(i);

	ctl_arena_clear(astats);
	ctl_arena_stats_amerge(tsdn, astats, arena);
	/* Merge into sum stats as well. */
	ctl_arena_stats_sdmerge(sdstats, astats, destroyed);
}

static unsigned
ctl_arena_init(tsdn_t *tsdn, extent_hooks_t *extent_hooks)
{
	unsigned arena_ind;
	ctl_arena_stats_t *astats;

	if ((astats = ql_last(&ctl_stats->destroyed, destroyed_link)) != NULL) {
		ql_remove(&ctl_stats->destroyed, astats, destroyed_link);
		arena_ind = astats->arena_ind;
	} else
		arena_ind = ctl_stats->narenas;

	/* Trigger stats allocation. */
	if (stats_arenas_i_impl(tsdn, arena_ind, false, true) == NULL)
		return (UINT_MAX);

	/* Initialize new arena. */
	if (arena_init(tsdn, arena_ind, extent_hooks) == NULL)
		return (UINT_MAX);

	if (arena_ind == ctl_stats->narenas)
		ctl_stats->narenas++;

	return (arena_ind);
}

static void
ctl_refresh(tsdn_t *tsdn)
{
	unsigned i;
	ctl_arena_stats_t *sstats = stats_arenas_i(MALLCTL_ARENAS_ALL);
	VARIABLE_ARRAY(arena_t *, tarenas, ctl_stats->narenas);

	/*
	 * Clear sum stats, since they will be merged into by
	 * ctl_arena_refresh().
	 */
	ctl_arena_clear(sstats);

	for (i = 0; i < ctl_stats->narenas; i++)
		tarenas[i] = arena_get(tsdn, i, false);

	for (i = 0; i < ctl_stats->narenas; i++) {
		ctl_arena_stats_t *astats = stats_arenas_i(i);
		bool initialized = (tarenas[i] != NULL);

		astats->initialized = initialized;
		if (initialized)
			ctl_arena_refresh(tsdn, tarenas[i], sstats, i, false);
	}

	if (config_stats) {
		ctl_stats->allocated = sstats->allocated_small +
		    sstats->astats.allocated_large;
		ctl_stats->active = (sstats->pactive << LG_PAGE);
		ctl_stats->metadata = sstats->astats.base +
		    sstats->astats.internal;
		ctl_stats->resident = sstats->astats.resident;
		ctl_stats->mapped = sstats->astats.mapped;
		ctl_stats->retained = sstats->astats.retained;
	}

	ctl_stats->epoch++;
}

static bool
ctl_init(tsdn_t *tsdn)
{
	bool ret;

	malloc_mutex_lock(tsdn, &ctl_mtx);
	if (!ctl_initialized) {
		ctl_arena_stats_t *sstats, *dstats;
		unsigned i;

		/*
		 * Allocate demand-zeroed space for pointers to the full range
		 * of supported arena indices.
		 */
		if (ctl_stats == NULL) {
			ctl_stats = (ctl_stats_t *)base_alloc(tsdn, b0get(),
			    sizeof(ctl_stats_t), QUANTUM);
			if (ctl_stats == NULL) {
				ret = true;
				goto label_return;
			}
		}

		/*
		 * Allocate space for the current full range of arenas here
		 * rather than doing it lazily elsewhere, in order to limit when
		 * OOM-caused errors can occur.
		 */
		if ((sstats = stats_arenas_i_impl(tsdn, MALLCTL_ARENAS_ALL,
		    false, true)) == NULL) {
			ret = true;
			goto label_return;
		}
		sstats->initialized = true;

		if ((dstats = stats_arenas_i_impl(tsdn,
		    MALLCTL_ARENAS_DESTROYED, false, true)) == NULL) {
			ret = true;
			goto label_return;
		}
		ctl_arena_clear(dstats);
		/*
		 * Don't toggle stats for MALLCTL_ARENAS_DESTROYED to
		 * initialized until an arena is actually destroyed, so that
		 * arena.<i>.initialized can be used to query whether the stats
		 * are relevant.
		 */

		ctl_stats->narenas = narenas_total_get();
		for (i = 0; i < ctl_stats->narenas; i++) {
			if (stats_arenas_i_impl(tsdn, i, false, true) == NULL) {
				ret = true;
				goto label_return;
			}
		}

		ql_new(&ctl_stats->destroyed);
		ctl_refresh(tsdn);
		ctl_initialized = true;
	}

	ret = false;
label_return:
	malloc_mutex_unlock(tsdn, &ctl_mtx);
	return (ret);
}

static int
ctl_lookup(tsdn_t *tsdn, const char *name, ctl_node_t const **nodesp,
    size_t *mibp, size_t *depthp)
{
	int ret;
	const char *elm, *tdot, *dot;
	size_t elen, i, j;
	const ctl_named_node_t *node;

	elm = name;
	/* Equivalent to strchrnul(). */
	dot = ((tdot = strchr(elm, '.')) != NULL) ? tdot : strchr(elm, '\0');
	elen = (size_t)((uintptr_t)dot - (uintptr_t)elm);
	if (elen == 0) {
		ret = ENOENT;
		goto label_return;
	}
	node = super_root_node;
	for (i = 0; i < *depthp; i++) {
		assert(node);
		assert(node->nchildren > 0);
		if (ctl_named_node(node->children) != NULL) {
			const ctl_named_node_t *pnode = node;

			/* Children are named. */
			for (j = 0; j < node->nchildren; j++) {
				const ctl_named_node_t *child =
				    ctl_named_children(node, j);
				if (strlen(child->name) == elen &&
				    strncmp(elm, child->name, elen) == 0) {
					node = child;
					if (nodesp != NULL)
						nodesp[i] =
						    (const ctl_node_t *)node;
					mibp[i] = j;
					break;
				}
			}
			if (node == pnode) {
				ret = ENOENT;
				goto label_return;
			}
		} else {
			uintmax_t index;
			const ctl_indexed_node_t *inode;

			/* Children are indexed. */
			index = malloc_strtoumax(elm, NULL, 10);
			if (index == UINTMAX_MAX || index > SIZE_T_MAX) {
				ret = ENOENT;
				goto label_return;
			}

			inode = ctl_indexed_node(node->children);
			node = inode->index(tsdn, mibp, *depthp, (size_t)index);
			if (node == NULL) {
				ret = ENOENT;
				goto label_return;
			}

			if (nodesp != NULL)
				nodesp[i] = (const ctl_node_t *)node;
			mibp[i] = (size_t)index;
		}

		if (node->ctl != NULL) {
			/* Terminal node. */
			if (*dot != '\0') {
				/*
				 * The name contains more elements than are
				 * in this path through the tree.
				 */
				ret = ENOENT;
				goto label_return;
			}
			/* Complete lookup successful. */
			*depthp = i + 1;
			break;
		}

		/* Update elm. */
		if (*dot == '\0') {
			/* No more elements. */
			ret = ENOENT;
			goto label_return;
		}
		elm = &dot[1];
		dot = ((tdot = strchr(elm, '.')) != NULL) ? tdot :
		    strchr(elm, '\0');
		elen = (size_t)((uintptr_t)dot - (uintptr_t)elm);
	}

	ret = 0;
label_return:
	return (ret);
}

int
ctl_byname(tsd_t *tsd, const char *name, void *oldp, size_t *oldlenp,
    void *newp, size_t newlen)
{
	int ret;
	size_t depth;
	ctl_node_t const *nodes[CTL_MAX_DEPTH];
	size_t mib[CTL_MAX_DEPTH];
	const ctl_named_node_t *node;

	if (!ctl_initialized && ctl_init(tsd_tsdn(tsd))) {
		ret = EAGAIN;
		goto label_return;
	}

	depth = CTL_MAX_DEPTH;
	ret = ctl_lookup(tsd_tsdn(tsd), name, nodes, mib, &depth);
	if (ret != 0)
		goto label_return;

	node = ctl_named_node(nodes[depth-1]);
	if (node != NULL && node->ctl)
		ret = node->ctl(tsd, mib, depth, oldp, oldlenp, newp, newlen);
	else {
		/* The name refers to a partial path through the ctl tree. */
		ret = ENOENT;
	}

label_return:
	return(ret);
}

int
ctl_nametomib(tsdn_t *tsdn, const char *name, size_t *mibp, size_t *miblenp)
{
	int ret;

	if (!ctl_initialized && ctl_init(tsdn)) {
		ret = EAGAIN;
		goto label_return;
	}

	ret = ctl_lookup(tsdn, name, NULL, mibp, miblenp);
label_return:
	return(ret);
}

int
ctl_bymib(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	const ctl_named_node_t *node;
	size_t i;

	if (!ctl_initialized && ctl_init(tsd_tsdn(tsd))) {
		ret = EAGAIN;
		goto label_return;
	}

	/* Iterate down the tree. */
	node = super_root_node;
	for (i = 0; i < miblen; i++) {
		assert(node);
		assert(node->nchildren > 0);
		if (ctl_named_node(node->children) != NULL) {
			/* Children are named. */
			if (node->nchildren <= mib[i]) {
				ret = ENOENT;
				goto label_return;
			}
			node = ctl_named_children(node, mib[i]);
		} else {
			const ctl_indexed_node_t *inode;

			/* Indexed element. */
			inode = ctl_indexed_node(node->children);
			node = inode->index(tsd_tsdn(tsd), mib, miblen, mib[i]);
			if (node == NULL) {
				ret = ENOENT;
				goto label_return;
			}
		}
	}

	/* Call the ctl function. */
	if (node && node->ctl)
		ret = node->ctl(tsd, mib, miblen, oldp, oldlenp, newp, newlen);
	else {
		/* Partial MIB. */
		ret = ENOENT;
	}

label_return:
	return(ret);
}

bool
ctl_boot(void)
{
	if (malloc_mutex_init(&ctl_mtx, "ctl", WITNESS_RANK_CTL))
		return (true);

	ctl_initialized = false;

	return (false);
}

void
ctl_prefork(tsdn_t *tsdn)
{
	malloc_mutex_prefork(tsdn, &ctl_mtx);
}

void
ctl_postfork_parent(tsdn_t *tsdn)
{
	malloc_mutex_postfork_parent(tsdn, &ctl_mtx);
}

void
ctl_postfork_child(tsdn_t *tsdn)
{
	malloc_mutex_postfork_child(tsdn, &ctl_mtx);
}

/******************************************************************************/
/* *_ctl() functions. */

#define	READONLY()	do {						\
	if (newp != NULL || newlen != 0) {				\
		ret = EPERM;						\
		goto label_return;					\
	}								\
} while (0)

#define	WRITEONLY()	do {						\
	if (oldp != NULL || oldlenp != NULL) {				\
		ret = EPERM;						\
		goto label_return;					\
	}								\
} while (0)

#define	READ_XOR_WRITE()	do {					\
	if ((oldp != NULL && oldlenp != NULL) && (newp != NULL ||	\
	    newlen != 0)) {						\
		ret = EPERM;						\
		goto label_return;					\
	}								\
} while (0)

#define	READ(v, t)	do {						\
	if (oldp != NULL && oldlenp != NULL) {				\
		if (*oldlenp != sizeof(t)) {				\
			size_t	copylen = (sizeof(t) <= *oldlenp)	\
			    ? sizeof(t) : *oldlenp;			\
			memcpy(oldp, (void *)&(v), copylen);		\
			ret = EINVAL;					\
			goto label_return;				\
		}							\
		*(t *)oldp = (v);					\
	}								\
} while (0)

#define	WRITE(v, t)	do {						\
	if (newp != NULL) {						\
		if (newlen != sizeof(t)) {				\
			ret = EINVAL;					\
			goto label_return;				\
		}							\
		(v) = *(t *)newp;					\
	}								\
} while (0)

#define	MIB_UNSIGNED(v, i) do {						\
	if (mib[i] > UINT_MAX) {					\
		ret = EFAULT;						\
		goto label_return;					\
	}								\
	v = (unsigned)mib[i];						\
} while (0)

/*
 * There's a lot of code duplication in the following macros due to limitations
 * in how nested cpp macros are expanded.
 */
#define	CTL_RO_CLGEN(c, l, n, v, t)					\
static int								\
n##_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,	\
    size_t *oldlenp, void *newp, size_t newlen)				\
{									\
	int ret;							\
	t oldval;							\
									\
	if (!(c))							\
		return (ENOENT);					\
	if (l)								\
		malloc_mutex_lock(tsd_tsdn(tsd), &ctl_mtx);		\
	READONLY();							\
	oldval = (v);							\
	READ(oldval, t);						\
									\
	ret = 0;							\
label_return:								\
	if (l)								\
		malloc_mutex_unlock(tsd_tsdn(tsd), &ctl_mtx);		\
	return (ret);							\
}

#define	CTL_RO_CGEN(c, n, v, t)						\
static int								\
n##_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,	\
    size_t *oldlenp, void *newp, size_t newlen)				\
{									\
	int ret;							\
	t oldval;							\
									\
	if (!(c))							\
		return (ENOENT);					\
	malloc_mutex_lock(tsd_tsdn(tsd), &ctl_mtx);			\
	READONLY();							\
	oldval = (v);							\
	READ(oldval, t);						\
									\
	ret = 0;							\
label_return:								\
	malloc_mutex_unlock(tsd_tsdn(tsd), &ctl_mtx);			\
	return (ret);							\
}

#define	CTL_RO_GEN(n, v, t)						\
static int								\
n##_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,	\
    size_t *oldlenp, void *newp, size_t newlen)				\
{									\
	int ret;							\
	t oldval;							\
									\
	malloc_mutex_lock(tsd_tsdn(tsd), &ctl_mtx);			\
	READONLY();							\
	oldval = (v);							\
	READ(oldval, t);						\
									\
	ret = 0;							\
label_return:								\
	malloc_mutex_unlock(tsd_tsdn(tsd), &ctl_mtx);			\
	return (ret);							\
}

/*
 * ctl_mtx is not acquired, under the assumption that no pertinent data will
 * mutate during the call.
 */
#define	CTL_RO_NL_CGEN(c, n, v, t)					\
static int								\
n##_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,	\
    size_t *oldlenp, void *newp, size_t newlen)				\
{									\
	int ret;							\
	t oldval;							\
									\
	if (!(c))							\
		return (ENOENT);					\
	READONLY();							\
	oldval = (v);							\
	READ(oldval, t);						\
									\
	ret = 0;							\
label_return:								\
	return (ret);							\
}

#define	CTL_RO_NL_GEN(n, v, t)						\
static int								\
n##_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,	\
    size_t *oldlenp, void *newp, size_t newlen)				\
{									\
	int ret;							\
	t oldval;							\
									\
	READONLY();							\
	oldval = (v);							\
	READ(oldval, t);						\
									\
	ret = 0;							\
label_return:								\
	return (ret);							\
}

#define	CTL_TSD_RO_NL_CGEN(c, n, m, t)					\
static int								\
n##_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,	\
    size_t *oldlenp, void *newp, size_t newlen)				\
{									\
	int ret;							\
	t oldval;							\
									\
	if (!(c))							\
		return (ENOENT);					\
	READONLY();							\
	oldval = (m(tsd));						\
	READ(oldval, t);						\
									\
	ret = 0;							\
label_return:								\
	return (ret);							\
}

#define	CTL_RO_CONFIG_GEN(n, t)						\
static int								\
n##_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,	\
    size_t *oldlenp, void *newp, size_t newlen)				\
{									\
	int ret;							\
	t oldval;							\
									\
	READONLY();							\
	oldval = n;							\
	READ(oldval, t);						\
									\
	ret = 0;							\
label_return:								\
	return (ret);							\
}

/******************************************************************************/

CTL_RO_NL_GEN(version, JEMALLOC_VERSION, const char *)

static int
epoch_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	UNUSED uint64_t newval;

	malloc_mutex_lock(tsd_tsdn(tsd), &ctl_mtx);
	WRITE(newval, uint64_t);
	if (newp != NULL)
		ctl_refresh(tsd_tsdn(tsd));
	READ(ctl_stats->epoch, uint64_t);

	ret = 0;
label_return:
	malloc_mutex_unlock(tsd_tsdn(tsd), &ctl_mtx);
	return (ret);
}

/******************************************************************************/

CTL_RO_CONFIG_GEN(config_cache_oblivious, bool)
CTL_RO_CONFIG_GEN(config_debug, bool)
CTL_RO_CONFIG_GEN(config_fill, bool)
CTL_RO_CONFIG_GEN(config_lazy_lock, bool)
CTL_RO_CONFIG_GEN(config_malloc_conf, const char *)
CTL_RO_CONFIG_GEN(config_munmap, bool)
CTL_RO_CONFIG_GEN(config_prof, bool)
CTL_RO_CONFIG_GEN(config_prof_libgcc, bool)
CTL_RO_CONFIG_GEN(config_prof_libunwind, bool)
CTL_RO_CONFIG_GEN(config_stats, bool)
CTL_RO_CONFIG_GEN(config_tcache, bool)
CTL_RO_CONFIG_GEN(config_tls, bool)
CTL_RO_CONFIG_GEN(config_utrace, bool)
CTL_RO_CONFIG_GEN(config_xmalloc, bool)

/******************************************************************************/

CTL_RO_NL_GEN(opt_abort, opt_abort, bool)
CTL_RO_NL_GEN(opt_dss, opt_dss, const char *)
CTL_RO_NL_GEN(opt_narenas, opt_narenas, unsigned)
CTL_RO_NL_GEN(opt_decay_time, opt_decay_time, ssize_t)
CTL_RO_NL_GEN(opt_stats_print, opt_stats_print, bool)
CTL_RO_NL_CGEN(config_fill, opt_junk, opt_junk, const char *)
CTL_RO_NL_CGEN(config_fill, opt_zero, opt_zero, bool)
CTL_RO_NL_CGEN(config_utrace, opt_utrace, opt_utrace, bool)
CTL_RO_NL_CGEN(config_xmalloc, opt_xmalloc, opt_xmalloc, bool)
CTL_RO_NL_CGEN(config_tcache, opt_tcache, opt_tcache, bool)
CTL_RO_NL_CGEN(config_tcache, opt_lg_tcache_max, opt_lg_tcache_max, ssize_t)
CTL_RO_NL_CGEN(config_prof, opt_prof, opt_prof, bool)
CTL_RO_NL_CGEN(config_prof, opt_prof_prefix, opt_prof_prefix, const char *)
CTL_RO_NL_CGEN(config_prof, opt_prof_active, opt_prof_active, bool)
CTL_RO_NL_CGEN(config_prof, opt_prof_thread_active_init,
    opt_prof_thread_active_init, bool)
CTL_RO_NL_CGEN(config_prof, opt_lg_prof_sample, opt_lg_prof_sample, size_t)
CTL_RO_NL_CGEN(config_prof, opt_prof_accum, opt_prof_accum, bool)
CTL_RO_NL_CGEN(config_prof, opt_lg_prof_interval, opt_lg_prof_interval, ssize_t)
CTL_RO_NL_CGEN(config_prof, opt_prof_gdump, opt_prof_gdump, bool)
CTL_RO_NL_CGEN(config_prof, opt_prof_final, opt_prof_final, bool)
CTL_RO_NL_CGEN(config_prof, opt_prof_leak, opt_prof_leak, bool)

/******************************************************************************/

static int
thread_arena_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	arena_t *oldarena;
	unsigned newind, oldind;

	oldarena = arena_choose(tsd, NULL);
	if (oldarena == NULL)
		return (EAGAIN);

	newind = oldind = arena_ind_get(oldarena);
	WRITE(newind, unsigned);
	READ(oldind, unsigned);
	if (newind != oldind) {
		arena_t *newarena;

		if (newind >= narenas_total_get()) {
			/* New arena index is out of range. */
			ret = EFAULT;
			goto label_return;
		}

		/* Initialize arena if necessary. */
		newarena = arena_get(tsd_tsdn(tsd), newind, true);
		if (newarena == NULL) {
			ret = EAGAIN;
			goto label_return;
		}
		/* Set new arena/tcache associations. */
		arena_migrate(tsd, oldind, newind);
		if (config_tcache) {
			tcache_t *tcache = tsd_tcache_get(tsd);
			if (tcache != NULL) {
				tcache_arena_reassociate(tsd_tsdn(tsd), tcache,
				    oldarena, newarena);
			}
		}
	}

	ret = 0;
label_return:
	return (ret);
}

CTL_TSD_RO_NL_CGEN(config_stats, thread_allocated, tsd_thread_allocated_get,
    uint64_t)
CTL_TSD_RO_NL_CGEN(config_stats, thread_allocatedp, tsd_thread_allocatedp_get,
    uint64_t *)
CTL_TSD_RO_NL_CGEN(config_stats, thread_deallocated, tsd_thread_deallocated_get,
    uint64_t)
CTL_TSD_RO_NL_CGEN(config_stats, thread_deallocatedp,
    tsd_thread_deallocatedp_get, uint64_t *)

static int
thread_tcache_enabled_ctl(tsd_t *tsd, const size_t *mib, size_t miblen,
    void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	bool oldval;

	if (!config_tcache)
		return (ENOENT);

	oldval = tcache_enabled_get();
	if (newp != NULL) {
		if (newlen != sizeof(bool)) {
			ret = EINVAL;
			goto label_return;
		}
		tcache_enabled_set(*(bool *)newp);
	}
	READ(oldval, bool);

	ret = 0;
label_return:
	return (ret);
}

static int
thread_tcache_flush_ctl(tsd_t *tsd, const size_t *mib, size_t miblen,
    void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;

	if (!config_tcache)
		return (ENOENT);

	READONLY();
	WRITEONLY();

	tcache_flush();

	ret = 0;
label_return:
	return (ret);
}

static int
thread_prof_name_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;

	if (!config_prof)
		return (ENOENT);

	READ_XOR_WRITE();

	if (newp != NULL) {
		if (newlen != sizeof(const char *)) {
			ret = EINVAL;
			goto label_return;
		}

		if ((ret = prof_thread_name_set(tsd, *(const char **)newp)) !=
		    0)
			goto label_return;
	} else {
		const char *oldname = prof_thread_name_get(tsd);
		READ(oldname, const char *);
	}

	ret = 0;
label_return:
	return (ret);
}

static int
thread_prof_active_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	bool oldval;

	if (!config_prof)
		return (ENOENT);

	oldval = prof_thread_active_get(tsd);
	if (newp != NULL) {
		if (newlen != sizeof(bool)) {
			ret = EINVAL;
			goto label_return;
		}
		if (prof_thread_active_set(tsd, *(bool *)newp)) {
			ret = EAGAIN;
			goto label_return;
		}
	}
	READ(oldval, bool);

	ret = 0;
label_return:
	return (ret);
}

/******************************************************************************/

static int
tcache_create_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	unsigned tcache_ind;

	if (!config_tcache)
		return (ENOENT);

	malloc_mutex_lock(tsd_tsdn(tsd), &ctl_mtx);
	READONLY();
	if (tcaches_create(tsd, &tcache_ind)) {
		ret = EFAULT;
		goto label_return;
	}
	READ(tcache_ind, unsigned);

	ret = 0;
label_return:
	malloc_mutex_unlock(tsd_tsdn(tsd), &ctl_mtx);
	return (ret);
}

static int
tcache_flush_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	unsigned tcache_ind;

	if (!config_tcache)
		return (ENOENT);

	WRITEONLY();
	tcache_ind = UINT_MAX;
	WRITE(tcache_ind, unsigned);
	if (tcache_ind == UINT_MAX) {
		ret = EFAULT;
		goto label_return;
	}
	tcaches_flush(tsd, tcache_ind);

	ret = 0;
label_return:
	return (ret);
}

static int
tcache_destroy_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	unsigned tcache_ind;

	if (!config_tcache)
		return (ENOENT);

	WRITEONLY();
	tcache_ind = UINT_MAX;
	WRITE(tcache_ind, unsigned);
	if (tcache_ind == UINT_MAX) {
		ret = EFAULT;
		goto label_return;
	}
	tcaches_destroy(tsd, tcache_ind);

	ret = 0;
label_return:
	return (ret);
}

/******************************************************************************/

static int
arena_i_initialized_ctl(tsd_t *tsd, const size_t *mib, size_t miblen,
    void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	tsdn_t *tsdn = tsd_tsdn(tsd);
	unsigned arena_ind;
	bool initialized;

	READONLY();
	MIB_UNSIGNED(arena_ind, 1);

	malloc_mutex_lock(tsdn, &ctl_mtx);
	initialized = stats_arenas_i(arena_ind)->initialized;
	malloc_mutex_unlock(tsdn, &ctl_mtx);

	READ(initialized, bool);

	ret = 0;
label_return:
	return (ret);
}

static void
arena_i_purge(tsdn_t *tsdn, unsigned arena_ind, bool all)
{
	malloc_mutex_lock(tsdn, &ctl_mtx);
	{
		unsigned narenas = ctl_stats->narenas;

		/*
		 * Access via index narenas is deprecated, and scheduled for
		 * removal in 6.0.0.
		 */
		if (arena_ind == MALLCTL_ARENAS_ALL || arena_ind == narenas) {
			unsigned i;
			VARIABLE_ARRAY(arena_t *, tarenas, narenas);

			for (i = 0; i < narenas; i++)
				tarenas[i] = arena_get(tsdn, i, false);

			/*
			 * No further need to hold ctl_mtx, since narenas and
			 * tarenas contain everything needed below.
			 */
			malloc_mutex_unlock(tsdn, &ctl_mtx);

			for (i = 0; i < narenas; i++) {
				if (tarenas[i] != NULL)
					arena_purge(tsdn, tarenas[i], all);
			}
		} else {
			arena_t *tarena;

			assert(arena_ind < narenas);

			tarena = arena_get(tsdn, arena_ind, false);

			/* No further need to hold ctl_mtx. */
			malloc_mutex_unlock(tsdn, &ctl_mtx);

			if (tarena != NULL)
				arena_purge(tsdn, tarena, all);
		}
	}
}

static int
arena_i_purge_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	unsigned arena_ind;

	READONLY();
	WRITEONLY();
	MIB_UNSIGNED(arena_ind, 1);
	arena_i_purge(tsd_tsdn(tsd), arena_ind, true);

	ret = 0;
label_return:
	return (ret);
}

static int
arena_i_decay_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	unsigned arena_ind;

	READONLY();
	WRITEONLY();
	MIB_UNSIGNED(arena_ind, 1);
	arena_i_purge(tsd_tsdn(tsd), arena_ind, false);

	ret = 0;
label_return:
	return (ret);
}

static int
arena_i_reset_destroy_helper(tsd_t *tsd, const size_t *mib, size_t miblen,
    void *oldp, size_t *oldlenp, void *newp, size_t newlen, unsigned *arena_ind,
    arena_t **arena)
{
	int ret;

	READONLY();
	WRITEONLY();
	MIB_UNSIGNED(*arena_ind, 1);

	if (*arena_ind < narenas_auto) {
		ret = EFAULT;
		goto label_return;
	}

	*arena = arena_get(tsd_tsdn(tsd), *arena_ind, false);
	if (*arena == NULL) {
		ret = EFAULT;
		goto label_return;
	}

	ret = 0;
label_return:
	return (ret);
}

static int
arena_i_reset_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	unsigned arena_ind;
	arena_t *arena;

	ret = arena_i_reset_destroy_helper(tsd, mib, miblen, oldp, oldlenp,
	    newp, newlen, &arena_ind, &arena);
	if (ret != 0)
		return (ret);

	arena_reset(tsd, arena);

	return (ret);
}

static int
arena_i_destroy_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	unsigned arena_ind;
	arena_t *arena;
	ctl_arena_stats_t *dstats, *astats;

	ret = arena_i_reset_destroy_helper(tsd, mib, miblen, oldp, oldlenp,
	    newp, newlen, &arena_ind, &arena);
	if (ret != 0)
		goto label_return;

	if (arena_nthreads_get(arena, false) != 0 || arena_nthreads_get(arena,
	    true) != 0) {
		ret = EFAULT;
		goto label_return;
	}

	/* Merge stats after resetting and purging arena. */
	arena_reset(tsd, arena);
	arena_purge(tsd_tsdn(tsd), arena, true);
	dstats = stats_arenas_i(MALLCTL_ARENAS_DESTROYED);
	dstats->initialized = true;
	ctl_arena_refresh(tsd_tsdn(tsd), arena, dstats, arena_ind, true);
	/* Destroy arena. */
	arena_destroy(tsd, arena);
	astats = stats_arenas_i(arena_ind);
	astats->initialized = false;
	/* Record arena index for later recycling via arenas.create. */
	ql_elm_new(astats, destroyed_link);
	ql_tail_insert(&ctl_stats->destroyed, astats, destroyed_link);

	assert(ret == 0);
label_return:
	return (ret);
}

static int
arena_i_dss_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	const char *dss = NULL;
	unsigned arena_ind;
	dss_prec_t dss_prec_old = dss_prec_limit;
	dss_prec_t dss_prec = dss_prec_limit;

	malloc_mutex_lock(tsd_tsdn(tsd), &ctl_mtx);
	WRITE(dss, const char *);
	MIB_UNSIGNED(arena_ind, 1);
	if (dss != NULL) {
		int i;
		bool match = false;

		for (i = 0; i < dss_prec_limit; i++) {
			if (strcmp(dss_prec_names[i], dss) == 0) {
				dss_prec = i;
				match = true;
				break;
			}
		}

		if (!match) {
			ret = EINVAL;
			goto label_return;
		}
	}

	/*
	 * Access via index narenas is deprecated, and scheduled for removal in
	 * 6.0.0.
	 */
	if (arena_ind == MALLCTL_ARENAS_ALL || arena_ind ==
	    ctl_stats->narenas) {
		if (dss_prec != dss_prec_limit &&
		    extent_dss_prec_set(dss_prec)) {
			ret = EFAULT;
			goto label_return;
		}
		dss_prec_old = extent_dss_prec_get();
	} else {
		arena_t *arena = arena_get(tsd_tsdn(tsd), arena_ind, false);
		if (arena == NULL || (dss_prec != dss_prec_limit &&
		    arena_dss_prec_set(tsd_tsdn(tsd), arena, dss_prec))) {
			ret = EFAULT;
			goto label_return;
		}
		dss_prec_old = arena_dss_prec_get(tsd_tsdn(tsd), arena);
	}

	dss = dss_prec_names[dss_prec_old];
	READ(dss, const char *);

	ret = 0;
label_return:
	malloc_mutex_unlock(tsd_tsdn(tsd), &ctl_mtx);
	return (ret);
}

static int
arena_i_decay_time_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	unsigned arena_ind;
	arena_t *arena;

	MIB_UNSIGNED(arena_ind, 1);
	arena = arena_get(tsd_tsdn(tsd), arena_ind, false);
	if (arena == NULL) {
		ret = EFAULT;
		goto label_return;
	}

	if (oldp != NULL && oldlenp != NULL) {
		size_t oldval = arena_decay_time_get(tsd_tsdn(tsd), arena);
		READ(oldval, ssize_t);
	}
	if (newp != NULL) {
		if (newlen != sizeof(ssize_t)) {
			ret = EINVAL;
			goto label_return;
		}
		if (arena_decay_time_set(tsd_tsdn(tsd), arena,
		    *(ssize_t *)newp)) {
			ret = EFAULT;
			goto label_return;
		}
	}

	ret = 0;
label_return:
	return (ret);
}

static int
arena_i_extent_hooks_ctl(tsd_t *tsd, const size_t *mib, size_t miblen,
    void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	unsigned arena_ind;
	arena_t *arena;

	malloc_mutex_lock(tsd_tsdn(tsd), &ctl_mtx);
	MIB_UNSIGNED(arena_ind, 1);
	if (arena_ind < narenas_total_get() && (arena =
	    arena_get(tsd_tsdn(tsd), arena_ind, false)) != NULL) {
		if (newp != NULL) {
			extent_hooks_t *old_extent_hooks;
			extent_hooks_t *new_extent_hooks
			    JEMALLOC_CC_SILENCE_INIT(NULL);
			WRITE(new_extent_hooks, extent_hooks_t *);
			old_extent_hooks = extent_hooks_set(arena,
			    new_extent_hooks);
			READ(old_extent_hooks, extent_hooks_t *);
		} else {
			extent_hooks_t *old_extent_hooks =
			    extent_hooks_get(arena);
			READ(old_extent_hooks, extent_hooks_t *);
		}
	} else {
		ret = EFAULT;
		goto label_return;
	}
	ret = 0;
label_return:
	malloc_mutex_unlock(tsd_tsdn(tsd), &ctl_mtx);
	return (ret);
}

static const ctl_named_node_t *
arena_i_index(tsdn_t *tsdn, const size_t *mib, size_t miblen, size_t i)
{
	const ctl_named_node_t *ret;

	malloc_mutex_lock(tsdn, &ctl_mtx);
	switch (i) {
	case MALLCTL_ARENAS_ALL:
	case MALLCTL_ARENAS_DESTROYED:
		break;
	default:
		if (i > ctl_stats->narenas) {
			ret = NULL;
			goto label_return;
		}
		break;
	}

	ret = super_arena_i_node;
label_return:
	malloc_mutex_unlock(tsdn, &ctl_mtx);
	return (ret);
}

/******************************************************************************/

static int
arenas_narenas_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	unsigned narenas;

	malloc_mutex_lock(tsd_tsdn(tsd), &ctl_mtx);
	READONLY();
	if (*oldlenp != sizeof(unsigned)) {
		ret = EINVAL;
		goto label_return;
	}
	narenas = ctl_stats->narenas;
	READ(narenas, unsigned);

	ret = 0;
label_return:
	malloc_mutex_unlock(tsd_tsdn(tsd), &ctl_mtx);
	return (ret);
}

static int
arenas_decay_time_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;

	if (oldp != NULL && oldlenp != NULL) {
		size_t oldval = arena_decay_time_default_get();
		READ(oldval, ssize_t);
	}
	if (newp != NULL) {
		if (newlen != sizeof(ssize_t)) {
			ret = EINVAL;
			goto label_return;
		}
		if (arena_decay_time_default_set(*(ssize_t *)newp)) {
			ret = EFAULT;
			goto label_return;
		}
	}

	ret = 0;
label_return:
	return (ret);
}

CTL_RO_NL_GEN(arenas_quantum, QUANTUM, size_t)
CTL_RO_NL_GEN(arenas_page, PAGE, size_t)
CTL_RO_NL_CGEN(config_tcache, arenas_tcache_max, tcache_maxclass, size_t)
CTL_RO_NL_GEN(arenas_nbins, NBINS, unsigned)
CTL_RO_NL_CGEN(config_tcache, arenas_nhbins, nhbins, unsigned)
CTL_RO_NL_GEN(arenas_bin_i_size, arena_bin_info[mib[2]].reg_size, size_t)
CTL_RO_NL_GEN(arenas_bin_i_nregs, arena_bin_info[mib[2]].nregs, uint32_t)
CTL_RO_NL_GEN(arenas_bin_i_slab_size, arena_bin_info[mib[2]].slab_size, size_t)
static const ctl_named_node_t *
arenas_bin_i_index(tsdn_t *tsdn, const size_t *mib, size_t miblen, size_t i)
{
	if (i > NBINS)
		return (NULL);
	return (super_arenas_bin_i_node);
}

CTL_RO_NL_GEN(arenas_nlextents, NSIZES - NBINS, unsigned)
CTL_RO_NL_GEN(arenas_lextent_i_size, index2size(NBINS+(szind_t)mib[2]), size_t)
static const ctl_named_node_t *
arenas_lextent_i_index(tsdn_t *tsdn, const size_t *mib, size_t miblen, size_t i)
{
	if (i > NSIZES - NBINS)
		return (NULL);
	return (super_arenas_lextent_i_node);
}

static int
arenas_create_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	extent_hooks_t *extent_hooks;
	unsigned arena_ind;

	malloc_mutex_lock(tsd_tsdn(tsd), &ctl_mtx);

	extent_hooks = (extent_hooks_t *)&extent_hooks_default;
	WRITE(extent_hooks, extent_hooks_t *);
	if ((arena_ind = ctl_arena_init(tsd_tsdn(tsd), extent_hooks)) ==
	    UINT_MAX) {
		ret = EAGAIN;
		goto label_return;
	}
	READ(arena_ind, unsigned);

	ret = 0;
label_return:
	malloc_mutex_unlock(tsd_tsdn(tsd), &ctl_mtx);
	return (ret);
}

/******************************************************************************/

static int
prof_thread_active_init_ctl(tsd_t *tsd, const size_t *mib, size_t miblen,
    void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	bool oldval;

	if (!config_prof)
		return (ENOENT);

	if (newp != NULL) {
		if (newlen != sizeof(bool)) {
			ret = EINVAL;
			goto label_return;
		}
		oldval = prof_thread_active_init_set(tsd_tsdn(tsd),
		    *(bool *)newp);
	} else
		oldval = prof_thread_active_init_get(tsd_tsdn(tsd));
	READ(oldval, bool);

	ret = 0;
label_return:
	return (ret);
}

static int
prof_active_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	bool oldval;

	if (!config_prof)
		return (ENOENT);

	if (newp != NULL) {
		if (newlen != sizeof(bool)) {
			ret = EINVAL;
			goto label_return;
		}
		oldval = prof_active_set(tsd_tsdn(tsd), *(bool *)newp);
	} else
		oldval = prof_active_get(tsd_tsdn(tsd));
	READ(oldval, bool);

	ret = 0;
label_return:
	return (ret);
}

static int
prof_dump_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	const char *filename = NULL;

	if (!config_prof)
		return (ENOENT);

	WRITEONLY();
	WRITE(filename, const char *);

	if (prof_mdump(tsd, filename)) {
		ret = EFAULT;
		goto label_return;
	}

	ret = 0;
label_return:
	return (ret);
}

static int
prof_gdump_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	bool oldval;

	if (!config_prof)
		return (ENOENT);

	if (newp != NULL) {
		if (newlen != sizeof(bool)) {
			ret = EINVAL;
			goto label_return;
		}
		oldval = prof_gdump_set(tsd_tsdn(tsd), *(bool *)newp);
	} else
		oldval = prof_gdump_get(tsd_tsdn(tsd));
	READ(oldval, bool);

	ret = 0;
label_return:
	return (ret);
}

static int
prof_reset_ctl(tsd_t *tsd, const size_t *mib, size_t miblen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	int ret;
	size_t lg_sample = lg_prof_sample;

	if (!config_prof)
		return (ENOENT);

	WRITEONLY();
	WRITE(lg_sample, size_t);
	if (lg_sample >= (sizeof(uint64_t) << 3))
		lg_sample = (sizeof(uint64_t) << 3) - 1;

	prof_reset(tsd, lg_sample);

	ret = 0;
label_return:
	return (ret);
}

CTL_RO_NL_CGEN(config_prof, prof_interval, prof_interval, uint64_t)
CTL_RO_NL_CGEN(config_prof, lg_prof_sample, lg_prof_sample, size_t)

/******************************************************************************/

CTL_RO_CGEN(config_stats, stats_allocated, ctl_stats->allocated, size_t)
CTL_RO_CGEN(config_stats, stats_active, ctl_stats->active, size_t)
CTL_RO_CGEN(config_stats, stats_metadata, ctl_stats->metadata, size_t)
CTL_RO_CGEN(config_stats, stats_resident, ctl_stats->resident, size_t)
CTL_RO_CGEN(config_stats, stats_mapped, ctl_stats->mapped, size_t)
CTL_RO_CGEN(config_stats, stats_retained, ctl_stats->retained, size_t)

CTL_RO_GEN(stats_arenas_i_dss, stats_arenas_i(mib[2])->dss, const char *)
CTL_RO_GEN(stats_arenas_i_decay_time, stats_arenas_i(mib[2])->decay_time,
    ssize_t)
CTL_RO_GEN(stats_arenas_i_nthreads, stats_arenas_i(mib[2])->nthreads,
    unsigned)
CTL_RO_GEN(stats_arenas_i_pactive, stats_arenas_i(mib[2])->pactive, size_t)
CTL_RO_GEN(stats_arenas_i_pdirty, stats_arenas_i(mib[2])->pdirty, size_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_mapped,
    stats_arenas_i(mib[2])->astats.mapped, size_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_retained,
    stats_arenas_i(mib[2])->astats.retained, size_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_npurge,
    stats_arenas_i(mib[2])->astats.npurge, uint64_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_nmadvise,
    stats_arenas_i(mib[2])->astats.nmadvise, uint64_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_purged,
    stats_arenas_i(mib[2])->astats.purged, uint64_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_base,
    stats_arenas_i(mib[2])->astats.base, size_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_internal,
    stats_arenas_i(mib[2])->astats.internal, size_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_resident,
    stats_arenas_i(mib[2])->astats.resident, size_t)

CTL_RO_CGEN(config_stats, stats_arenas_i_small_allocated,
    stats_arenas_i(mib[2])->allocated_small, size_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_small_nmalloc,
    stats_arenas_i(mib[2])->nmalloc_small, uint64_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_small_ndalloc,
    stats_arenas_i(mib[2])->ndalloc_small, uint64_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_small_nrequests,
    stats_arenas_i(mib[2])->nrequests_small, uint64_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_large_allocated,
    stats_arenas_i(mib[2])->astats.allocated_large, size_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_large_nmalloc,
    stats_arenas_i(mib[2])->astats.nmalloc_large, uint64_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_large_ndalloc,
    stats_arenas_i(mib[2])->astats.ndalloc_large, uint64_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_large_nrequests,
    stats_arenas_i(mib[2])->astats.nmalloc_large, uint64_t) /* Intentional. */

CTL_RO_CGEN(config_stats, stats_arenas_i_bins_j_nmalloc,
    stats_arenas_i(mib[2])->bstats[mib[4]].nmalloc, uint64_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_bins_j_ndalloc,
    stats_arenas_i(mib[2])->bstats[mib[4]].ndalloc, uint64_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_bins_j_nrequests,
    stats_arenas_i(mib[2])->bstats[mib[4]].nrequests, uint64_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_bins_j_curregs,
    stats_arenas_i(mib[2])->bstats[mib[4]].curregs, size_t)
CTL_RO_CGEN(config_stats && config_tcache, stats_arenas_i_bins_j_nfills,
    stats_arenas_i(mib[2])->bstats[mib[4]].nfills, uint64_t)
CTL_RO_CGEN(config_stats && config_tcache, stats_arenas_i_bins_j_nflushes,
    stats_arenas_i(mib[2])->bstats[mib[4]].nflushes, uint64_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_bins_j_nslabs,
    stats_arenas_i(mib[2])->bstats[mib[4]].nslabs, uint64_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_bins_j_nreslabs,
    stats_arenas_i(mib[2])->bstats[mib[4]].reslabs, uint64_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_bins_j_curslabs,
    stats_arenas_i(mib[2])->bstats[mib[4]].curslabs, size_t)

static const ctl_named_node_t *
stats_arenas_i_bins_j_index(tsdn_t *tsdn, const size_t *mib, size_t miblen,
    size_t j)
{
	if (j > NBINS)
		return (NULL);
	return (super_stats_arenas_i_bins_j_node);
}

CTL_RO_CGEN(config_stats, stats_arenas_i_lextents_j_nmalloc,
    stats_arenas_i(mib[2])->lstats[mib[4]].nmalloc, uint64_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_lextents_j_ndalloc,
    stats_arenas_i(mib[2])->lstats[mib[4]].ndalloc, uint64_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_lextents_j_nrequests,
    stats_arenas_i(mib[2])->lstats[mib[4]].nrequests, uint64_t)
CTL_RO_CGEN(config_stats, stats_arenas_i_lextents_j_curlextents,
    stats_arenas_i(mib[2])->lstats[mib[4]].curlextents, size_t)

static const ctl_named_node_t *
stats_arenas_i_lextents_j_index(tsdn_t *tsdn, const size_t *mib, size_t miblen,
    size_t j)
{
	if (j > NSIZES - NBINS)
		return (NULL);
	return (super_stats_arenas_i_lextents_j_node);
}

static const ctl_named_node_t *
stats_arenas_i_index(tsdn_t *tsdn, const size_t *mib, size_t miblen, size_t i)
{
	const ctl_named_node_t *ret;
	size_t a;

	malloc_mutex_lock(tsdn, &ctl_mtx);
	a = stats_arenas_i2a_impl(i, true, true);
	if (a == UINT_MAX || !ctl_stats->arenas[a]->initialized) {
		ret = NULL;
		goto label_return;
	}

	ret = super_stats_arenas_i_node;
label_return:
	malloc_mutex_unlock(tsdn, &ctl_mtx);
	return (ret);
}
