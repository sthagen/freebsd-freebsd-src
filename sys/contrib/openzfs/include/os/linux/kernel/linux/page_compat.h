#ifndef	_ZFS_PAGE_COMPAT_H
#define	_ZFS_PAGE_COMPAT_H

/*
 * Create our own accessor functions to follow the Linux API changes
 */
#define	nr_file_pages() (global_node_page_state(NR_ACTIVE_FILE) + \
			global_node_page_state(NR_INACTIVE_FILE))
#define	nr_inactive_file_pages() global_node_page_state(NR_INACTIVE_FILE)

#endif /* _ZFS_PAGE_COMPAT_H */
