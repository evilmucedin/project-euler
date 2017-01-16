#ifndef JEMALLOC_INTERNAL_PROF_INLINES_H
#define JEMALLOC_INTERNAL_PROF_INLINES_H

#ifndef JEMALLOC_ENABLE_INLINE
bool	prof_active_get_unlocked(void);
bool	prof_gdump_get_unlocked(void);
prof_tdata_t	*prof_tdata_get(tsd_t *tsd, bool create);
prof_tctx_t	*prof_tctx_get(tsdn_t *tsdn, const extent_t *extent,
    const void *ptr);
void	prof_tctx_set(tsdn_t *tsdn, extent_t *extent, const void *ptr,
    size_t usize, prof_tctx_t *tctx);
void	prof_tctx_reset(tsdn_t *tsdn, extent_t *extent, const void *ptr,
    prof_tctx_t *tctx);
bool	prof_sample_accum_update(tsd_t *tsd, size_t usize, bool update,
    prof_tdata_t **tdata_out);
prof_tctx_t	*prof_alloc_prep(tsd_t *tsd, size_t usize, bool prof_active,
    bool update);
void	prof_malloc(tsdn_t *tsdn, extent_t *extent, const void *ptr,
    size_t usize, prof_tctx_t *tctx);
void	prof_realloc(tsd_t *tsd, extent_t *extent, const void *ptr,
    size_t usize, prof_tctx_t *tctx, bool prof_active, bool updated,
    extent_t *old_extent, const void *old_ptr, size_t old_usize,
    prof_tctx_t *old_tctx);
void	prof_free(tsd_t *tsd, const extent_t *extent, const void *ptr,
    size_t usize);
#endif

#if (defined(JEMALLOC_ENABLE_INLINE) || defined(JEMALLOC_PROF_C_))
JEMALLOC_ALWAYS_INLINE bool
prof_active_get_unlocked(void)
{
	/*
	 * Even if opt_prof is true, sampling can be temporarily disabled by
	 * setting prof_active to false.  No locking is used when reading
	 * prof_active in the fast path, so there are no guarantees regarding
	 * how long it will take for all threads to notice state changes.
	 */
	return (prof_active);
}

JEMALLOC_ALWAYS_INLINE bool
prof_gdump_get_unlocked(void)
{
	/*
	 * No locking is used when reading prof_gdump_val in the fast path, so
	 * there are no guarantees regarding how long it will take for all
	 * threads to notice state changes.
	 */
	return (prof_gdump_val);
}

JEMALLOC_ALWAYS_INLINE prof_tdata_t *
prof_tdata_get(tsd_t *tsd, bool create)
{
	prof_tdata_t *tdata;

	cassert(config_prof);

	tdata = tsd_prof_tdata_get(tsd);
	if (create) {
		if (unlikely(tdata == NULL)) {
			if (tsd_nominal(tsd)) {
				tdata = prof_tdata_init(tsd);
				tsd_prof_tdata_set(tsd, tdata);
			}
		} else if (unlikely(tdata->expired)) {
			tdata = prof_tdata_reinit(tsd, tdata);
			tsd_prof_tdata_set(tsd, tdata);
		}
		assert(tdata == NULL || tdata->attached);
	}

	return (tdata);
}

JEMALLOC_ALWAYS_INLINE prof_tctx_t *
prof_tctx_get(tsdn_t *tsdn, const extent_t *extent, const void *ptr)
{
	cassert(config_prof);
	assert(ptr != NULL);

	return (arena_prof_tctx_get(tsdn, extent, ptr));
}

JEMALLOC_ALWAYS_INLINE void
prof_tctx_set(tsdn_t *tsdn, extent_t *extent, const void *ptr, size_t usize,
    prof_tctx_t *tctx)
{
	cassert(config_prof);
	assert(ptr != NULL);

	arena_prof_tctx_set(tsdn, extent, ptr, usize, tctx);
}

JEMALLOC_ALWAYS_INLINE void
prof_tctx_reset(tsdn_t *tsdn, extent_t *extent, const void *ptr,
    prof_tctx_t *tctx)
{
	cassert(config_prof);
	assert(ptr != NULL);

	arena_prof_tctx_reset(tsdn, extent, ptr, tctx);
}

JEMALLOC_ALWAYS_INLINE bool
prof_sample_accum_update(tsd_t *tsd, size_t usize, bool update,
    prof_tdata_t **tdata_out)
{
	prof_tdata_t *tdata;

	cassert(config_prof);

	tdata = prof_tdata_get(tsd, true);
	if (unlikely((uintptr_t)tdata <= (uintptr_t)PROF_TDATA_STATE_MAX))
		tdata = NULL;

	if (tdata_out != NULL)
		*tdata_out = tdata;

	if (unlikely(tdata == NULL))
		return (true);

	if (likely(tdata->bytes_until_sample >= usize)) {
		if (update)
			tdata->bytes_until_sample -= usize;
		return (true);
	} else {
		/* Compute new sample threshold. */
		if (update)
			prof_sample_threshold_update(tdata);
		return (!tdata->active);
	}
}

JEMALLOC_ALWAYS_INLINE prof_tctx_t *
prof_alloc_prep(tsd_t *tsd, size_t usize, bool prof_active, bool update)
{
	prof_tctx_t *ret;
	prof_tdata_t *tdata;
	prof_bt_t bt;

	assert(usize == s2u(usize));

	if (!prof_active || likely(prof_sample_accum_update(tsd, usize, update,
	    &tdata)))
		ret = (prof_tctx_t *)(uintptr_t)1U;
	else {
		bt_init(&bt, tdata->vec);
		prof_backtrace(&bt);
		ret = prof_lookup(tsd, &bt);
	}

	return (ret);
}

JEMALLOC_ALWAYS_INLINE void
prof_malloc(tsdn_t *tsdn, extent_t *extent, const void *ptr, size_t usize,
    prof_tctx_t *tctx)
{
	cassert(config_prof);
	assert(ptr != NULL);
	assert(usize == isalloc(tsdn, extent, ptr));

	if (unlikely((uintptr_t)tctx > (uintptr_t)1U))
		prof_malloc_sample_object(tsdn, extent, ptr, usize, tctx);
	else {
		prof_tctx_set(tsdn, extent, ptr, usize,
		    (prof_tctx_t *)(uintptr_t)1U);
	}
}

JEMALLOC_ALWAYS_INLINE void
prof_realloc(tsd_t *tsd, extent_t *extent, const void *ptr, size_t usize,
    prof_tctx_t *tctx, bool prof_active, bool updated, extent_t *old_extent,
    const void *old_ptr, size_t old_usize, prof_tctx_t *old_tctx)
{
	bool sampled, old_sampled, moved;

	cassert(config_prof);
	assert(ptr != NULL || (uintptr_t)tctx <= (uintptr_t)1U);

	if (prof_active && !updated && ptr != NULL) {
		assert(usize == isalloc(tsd_tsdn(tsd), extent, ptr));
		if (prof_sample_accum_update(tsd, usize, true, NULL)) {
			/*
			 * Don't sample.  The usize passed to prof_alloc_prep()
			 * was larger than what actually got allocated, so a
			 * backtrace was captured for this allocation, even
			 * though its actual usize was insufficient to cross the
			 * sample threshold.
			 */
			prof_alloc_rollback(tsd, tctx, true);
			tctx = (prof_tctx_t *)(uintptr_t)1U;
		}
	}

	/*
	 * The following code must differentiate among eight possible cases,
	 * based on three boolean conditions.
	 */
	sampled = ((uintptr_t)tctx > (uintptr_t)1U);
	old_sampled = ((uintptr_t)old_tctx > (uintptr_t)1U);
	moved = (ptr != old_ptr);

	/*
	 * The following block must only execute if this is a non-moving
	 * reallocation, because for moving reallocation the old allocation will
	 * be deallocated via a separate call.
	 */
	if (unlikely(old_sampled) && !moved)
		prof_free_sampled_object(tsd, old_usize, old_tctx);

	if (unlikely(sampled)) {
		prof_malloc_sample_object(tsd_tsdn(tsd), extent, ptr, usize,
		    tctx);
	} else if (moved) {
		prof_tctx_set(tsd_tsdn(tsd), extent, ptr, usize,
		    (prof_tctx_t *)(uintptr_t)1U);
	} else if (unlikely(old_sampled))
		prof_tctx_reset(tsd_tsdn(tsd), extent, ptr, tctx);
}

JEMALLOC_ALWAYS_INLINE void
prof_free(tsd_t *tsd, const extent_t *extent, const void *ptr, size_t usize)
{
	prof_tctx_t *tctx = prof_tctx_get(tsd_tsdn(tsd), extent, ptr);

	cassert(config_prof);
	assert(usize == isalloc(tsd_tsdn(tsd), extent, ptr));

	if (unlikely((uintptr_t)tctx > (uintptr_t)1U))
		prof_free_sampled_object(tsd, usize, tctx);
}
#endif

#endif /* JEMALLOC_INTERNAL_PROF_INLINES_H */
