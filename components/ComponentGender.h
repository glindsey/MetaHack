#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

/// Gender and sex of an entity.
class ComponentGender
{
public:

  friend void from_json(json const& j, ComponentGender& obj);
  friend void to_json(json& j, ComponentGender const& obj);

protected:

private:  
  /// The gender of this entity, if any.
  /// Gender can br any of the following:
  ///   - Agender
  ///   - Anywhere on a continuum between 100% male and 100% female, with a corresponding
  ///     continuum indicating gender fluidity
  ///
  /// Combinations of these two values allow for a wide range of identifications, e.g.:
  ///   - Fluidity =   0, Alignment = 100/0 --> Totally male
  ///   - Fluidity =   0, Alignment = 0/100 --> Totally female
  ///   - Fluidity =   0, Alignment = 80/20 --> Mostly male with some female identification
  ///   - Fluidity =   0, Alignment = 50/50 --> 50/50 intergender
  ///   - Fluidity = 100, Alignment = 50/50 --> 50/50 genderfluid
  ///
  /// This doesn't cover everything, but it does cover an awfully wide range of possibilities.

  /// The physical sex of this entity, if any.
  /// Also represented as an intensity/alignment pair.

  /// The attraction of this entity to particular genders.

  /// The attraction of this entity to particular physical sexes.

  /// The preferred pronouns for this entity.
};

