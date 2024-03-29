#ifndef UID_H
#define UID_H

struct UID {
	uint64_t value;

	UID();
	UID(const uint64_t& uuid);
	UID(uint64_t&& uuid);
	UID(const UID&) = default;

	UID& operator=(const UID& other);
	UID& operator=(UID&& other);

	UID& operator=(const uint64_t& other);
	UID& operator=(uint64_t&& other);

	bool is_valid() const { return value != 0; }

	operator uint64_t() const { return value; }
};

inline const UID INVALID_UID{ 0 };

namespace std {
template <typename T>
struct hash;

template <>
struct hash<UID> {
	size_t operator()(const UID& uuid) const { return (uint64_t)uuid; }
};
} //namespace std

#endif
