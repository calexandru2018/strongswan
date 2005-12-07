/**
 * @file ike_sa_manager.h
 * 
 * @brief Interface of ike_sa_manager_t.
 * 
 */

/*
 * Copyright (C) 2005 Jan Hutter, Martin Willi
 * Hochschule fuer Technik Rapperswil
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#ifndef _IKE_SA_MANAGER_H_
#define _IKE_SA_MANAGER_H_

#include <types.h>
#include <sa/ike_sa.h>


typedef struct ike_sa_manager_t ike_sa_manager_t;

/**
 * @brief The IKE_SA-Manager is responsible for managing all initiated and responded IKE_SA's.
 *
 * To avoid access from multiple threads, IKE_SAs must be checked out from
 * the manager, and checked in after usage. 
 * The manager also handles deletion of SAs.
 *
 * @todo checking of double-checkouts from the same threads would be nice.
 * This could be done by comparing thread-ids via pthread_self()...
 * 
 * @todo Managing of ike_sa_t objects in a hash table instead of linked list.
 * 
 * @b Constructors:
 * - ike_sa_manager_create()
 * 
 * @ingroup sa
 */
struct ike_sa_manager_t {
	/**
	 * @brief Checkout an IKE_SA, create it when necesarry.
	 * 
	 * Checks out a SA by its ID. An SA will be created, when:
	 * - Responder SPI is not set (when received an IKE_SA_INIT from initiator)
	 * Management of SPIs is the managers job, he will set it.
	 * This function blocks until SA is available for checkout.
	 * 
	 * @warning checking out two times without checking in will
	 * result in a deadlock!
	 * 
	 * @param ike_sa_manager 	the manager object
	 * @param ike_sa_id[in/out]	the SA identifier, will be updated
	 * @param ike_sa[out] 		checked out SA
	 * @returns 					
	 * 							- SUCCESS if checkout successful
	 * 							- NOT_FOUND when no such SA is available
	 * 							- CREATED if a new IKE_SA got created
	 */
	status_t (*checkout) (ike_sa_manager_t* ike_sa_manager, ike_sa_id_t *sa_id, ike_sa_t **ike_sa);
	
	/**
	 * @brief Create and checkout an IKE_SA as original initator.
	 * 
	 * Creates and checks out a SA as initiator.
	 * Management of SPIs is the managers job, he will set it.
	 * 
	 * @param ike_sa_manager 	the manager object
	 * @param ike_sa[out] 		checked out SA
	 */
	void (*create_and_checkout) (ike_sa_manager_t* ike_sa_manager,ike_sa_t **ike_sa);
 
	/**
	 * @brief Checkin the SA after usage.
	 * 
	 * @warning the SA pointer MUST NOT be used after checkin! 
	 * The SA must be checked out again!
	 *  
	 * @param ike_sa_manager 	the manager object
	 * @param ike_sa_id[in/out]	the SA identifier, will be updated
	 * @param ike_sa[out]		checked out SA
	 * @returns 				
	 * 							- SUCCESS if checked in
	 * 							- NOT_FOUND when not found (shouldn't happen!)
	 */
	status_t (*checkin) (ike_sa_manager_t* ike_sa_manager, ike_sa_t *ike_sa);
	/**
	 * @brief Delete a SA, which was not checked out.
	 * 
	 * @warning do not use this when the SA is already checked out, this will
	 * deadlock!
	 *  
	 * @param ike_sa_manager 	the manager object
	 * @param ike_sa_id[in/out]	the SA identifier
	 * @returns 				
	 * 							- SUCCESS if found
	 * 							- NOT_FOUND when no such SA is available
	 */
	status_t (*delete) (ike_sa_manager_t* ike_sa_manager, ike_sa_id_t *ike_sa_id);
	
	/**
	 * @brief Delete a checked out SA.
	 *
	 * @param ike_sa_manager 	the manager object
	 * @param ike_sa			SA to delete
	 * @returns 				
	 * 							- SUCCESS if found
	 * 							- NOT_FOUND when no such SA is available
	 */
	status_t (*checkin_and_delete) (ike_sa_manager_t* ike_sa_manager, ike_sa_t *ike_sa);
	
	/**
	 * @brief Destroys the manager with all associated SAs.
	 * 
	 * Threads will be driven out, so all SAs can be deleted cleanly.
	 * 
	 * @param ike_sa_manager the manager object
	 */
	void (*destroy) (ike_sa_manager_t *ike_sa_manager);
};

/**
 * @brief Create a manager.
 * 
 * @returns 	ike_sa_manager_t object
 * 
 * @ingroup sa
 */
ike_sa_manager_t *ike_sa_manager_create();

#endif /*_IKE_SA_MANAGER_H_*/
