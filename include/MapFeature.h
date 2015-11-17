#ifndef MAPFEATURE_H
#define MAPFEATURE_H

#include <memory>
#include <SFML/Graphics.hpp>

#include "Direction.h"
#include "GeoVector.h"
#include "Map.h"

/// Superclass for all map features (rooms, corridors, et cetera).  Maintains
/// a list of vectors at which new features can (theoretically) be attached.
class MapFeature
{
  public:
    MapFeature(Map& m, PropertyDictionary const& settings);
    virtual ~MapFeature();


    sf::IntRect const& get_coords() const;
    Map& get_map() const;
    PropertyDictionary const& get_settings() const;

    unsigned int get_num_growth_vectors() const;
    GeoVector const& get_random_growth_vector() const;
    bool erase_growth_vector(GeoVector vec);

    virtual bool create(GeoVector vec) = 0;

  protected:
    void set_coords(sf::IntRect coords);
    void clear_growth_vectors();
    void add_growth_vector(GeoVector vec);

  private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

#endif // MAPFEATURE_H
