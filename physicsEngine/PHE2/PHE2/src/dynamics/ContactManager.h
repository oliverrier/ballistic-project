#pragma once
#include <vector>
#include "../collision/BroadPhase.h"

class Contact;
class ContactFilter;
class ContactListener;

// Delegate of World.
class ContactManager
{
public:
	ContactManager();

	// Broad-phase callback.
	void AddPair(void* proxyUserDataA, void* proxyUserDataB);

	void FindNewContacts();

	void Destroy(Contact* c);

	void Collide();

	BroadPhase m_broadPhase;
	std::vector<Contact*> m_contactList;
	int m_contactCount;
	ContactFilter* m_contactFilter;
	ContactListener* m_contactListener;
};
