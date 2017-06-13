

#include "services/IStringDictionary.h"

// Using declarations
using StringMap = std::unordered_map<std::string, std::string>;

class StringDictionary : public IStringDictionary
{
public:
  /// Create a string dictionary, and optionally populate it from a file.
  /// @filename_ JSON file to populate the dictionary from, minus the
  ///            extension. 
  ///            If not present, the dictionary starts out empty.
  StringDictionary(std::string filename_ = "");
  ~StringDictionary();

  virtual bool loadFile(std::string filename_) override;
  virtual bool add(std::string id_, std::string str_) override;
  virtual void clear() override;
  virtual bool contains(std::string id_) const override;
  virtual std::string get(std::string id_) const override;

private:

  /// Data structure mapping string IDs to strings.
  StringMap	strings;
};