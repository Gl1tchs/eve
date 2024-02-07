namespace EveEngine
{
	public class Entity
	{
		public ulong Id { get; private set; }

		public Entity Parent
		{
			get
			{
				return new Entity(Interop.entity_get_parent(Id));
			}
		}

		public string Name
		{
			get => Interop.entity_get_name(Id);
		}

		public Transform Transform { get; private set; }

		public static Entity InvalidEntity => new();

		public Entity() { Id = 0; }

		public Entity(ulong id)
		{
			Id = id;
			Transform = GetComponent<Transform>();
		}

		protected virtual void OnCreate() { }

		protected virtual void OnUpdate(float dt) { }

		protected virtual void OnDestroy() { }

		public void Destroy()
		{
			Interop.entity_destroy(Id);

			Id = 0;
		}

		public bool HasComponent<T>() where T : Component, new()
		{
			return Interop.entity_has_component(Id, typeof(T));
		}

		public T GetComponent<T>() where T : Component, new()
		{
			if (!HasComponent<T>())
				return null;

			T component = new() { Entity = this };
			return component;
		}

		/// <exception cref="DuplicateComponentException">Will be thrown if component already exists.</exception>
		public T AddComponent<T>() where T : Component, new()
		{
			var componentType = typeof(T);

			if (HasComponent<T>())
			{
				throw new DuplicateComponentException(string.Format("Entity already has component of: {0}", componentType.FullName));
			}

			Interop.entity_add_component(Id, componentType);
			return GetComponent<T>();
		}

		public static Entity FindByName(string name)
		{
			ulong entityId = Interop.entity_find_by_name(name);
			return new Entity(entityId);
		}

		public T As<T>() where T : Entity, new()
		{
			object instance = Interop.get_script_instance(Id);
			return instance as T;
		}

		public T Instantiate<T>(string name, Vector3 position, Vector3 rotation, Vector3 scale) where T : Entity, new()
		{
			ulong createdId = Interop.entity_instantiate(name, 0, ref position, ref rotation, ref scale);

			// Only assign script if has it.
			if (typeof(T) != typeof(Entity))
			{
				Interop.entity_assign_script(createdId, typeof(T).FullName);
			}

			Entity createdEntity = new(createdId);
			return createdEntity as T;
		}

		public T Instantiate<T>(string name, Vector3 position, Vector3 rotation) where T : Entity, new()
		{
			return Instantiate<T>(name, Entity.InvalidEntity, position, rotation, Vector3.One);
		}

		public T Instantiate<T>(string name, Vector3 position) where T : Entity, new()
		{
			return Instantiate<T>(name, position, Vector3.Zero, Vector3.One);
		}

		public T Instantiate<T>(string name) where T : Entity, new()
		{
			return Instantiate<T>(name, Vector3.Zero, Vector3.Zero, Vector3.One);
		}

		public T Instantiate<T>(string name, Entity parent, Vector3 position, Vector3 rotation, Vector3 scale) where T : Entity, new()
		{
			ulong createdId = Interop.entity_instantiate(name, parent.Id, ref position, ref rotation, ref scale);

			// Only assign script if has it.
			if (typeof(T) != typeof(Entity))
			{
				Interop.entity_assign_script(createdId, typeof(T).FullName);
			}

			Entity createdEntity = new(createdId);
			return createdEntity as T;
		}

		public T Instantiate<T>(string name, Entity parent, Vector3 position, Vector3 rotation) where T : Entity, new()
		{
			return Instantiate<T>(name, parent, position, rotation, Vector3.One);
		}

		public T Instantiate<T>(string name, Entity parent, Vector3 position) where T : Entity, new()
		{
			return Instantiate<T>(name, parent, position, Vector3.Zero, Vector3.One);
		}

		public T Instantiate<T>(string name, Entity parent) where T : Entity, new()
		{
			return Instantiate<T>(name, parent, Vector3.Zero, Vector3.Zero, Vector3.One);
		}

		public override bool Equals(object obj)
		{
			if ((obj == null) || !this.GetType().Equals(obj.GetType()))
			{
				return false;
			}
			else
			{
				Entity entity = (Entity)obj;
				return Id == entity.Id;
			}
		}
		public override int GetHashCode()
		{
			return Id.GetHashCode();
		}

		public static implicit operator bool(Entity entity)
		{
			return entity.Id != 0;
		}
	}
}
