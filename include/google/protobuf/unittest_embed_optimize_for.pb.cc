// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: google/protobuf/unittest_embed_optimize_for.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include <google/protobuf/unittest_embed_optimize_for.pb.h>

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace protobuf_unittest {
class TestEmbedOptimizedForSizeDefaultTypeInternal : public ::google::protobuf::internal::ExplicitlyConstructed<TestEmbedOptimizedForSize> {
} _TestEmbedOptimizedForSize_default_instance_;

namespace protobuf_google_2fprotobuf_2funittest_5fembed_5foptimize_5ffor_2eproto {


namespace {

::google::protobuf::Metadata file_level_metadata[1];

}  // namespace

PROTOBUF_CONSTEXPR_VAR ::google::protobuf::internal::ParseTableField
    const TableStruct::entries[] = {
  {0, 0, 0, ::google::protobuf::internal::kInvalidMask, 0, 0},
};

PROTOBUF_CONSTEXPR_VAR ::google::protobuf::internal::AuxillaryParseTableField
    const TableStruct::aux[] = {
  ::google::protobuf::internal::AuxillaryParseTableField(),
};
PROTOBUF_CONSTEXPR_VAR ::google::protobuf::internal::ParseTable const
    TableStruct::schema[] = {
  { NULL, NULL, 0, -1, -1, false },
};

const ::google::protobuf::uint32 TableStruct::offsets[] = {
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TestEmbedOptimizedForSize, _has_bits_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TestEmbedOptimizedForSize, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TestEmbedOptimizedForSize, optional_message_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TestEmbedOptimizedForSize, repeated_message_),
  0,
  ~0u,
};

static const ::google::protobuf::internal::MigrationSchema schemas[] = {
  { 0, 7, sizeof(TestEmbedOptimizedForSize)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&_TestEmbedOptimizedForSize_default_instance_),
};

namespace {

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "google/protobuf/unittest_embed_optimize_for.proto", schemas, file_default_instances, TableStruct::offsets, factory,
      file_level_metadata, NULL, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::internal::RegisterAllTypes(file_level_metadata, 1);
}

}  // namespace

void TableStruct::Shutdown() {
  _TestEmbedOptimizedForSize_default_instance_.Shutdown();
  delete file_level_metadata[0].reflection;
}

void TableStruct::InitDefaultsImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::internal::InitProtobufDefaults();
  ::protobuf_unittest::protobuf_google_2fprotobuf_2funittest_5foptimize_5ffor_2eproto::InitDefaults();
  _TestEmbedOptimizedForSize_default_instance_.DefaultConstruct();
  _TestEmbedOptimizedForSize_default_instance_.get_mutable()->optional_message_ = const_cast< ::protobuf_unittest::TestOptimizedForSize*>(
      ::protobuf_unittest::TestOptimizedForSize::internal_default_instance());
}

void InitDefaults() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &TableStruct::InitDefaultsImpl);
}
void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] = {
      "\n1google/protobuf/unittest_embed_optimiz"
      "e_for.proto\022\021protobuf_unittest\032+google/p"
      "rotobuf/unittest_optimize_for.proto\"\241\001\n\031"
      "TestEmbedOptimizedForSize\022A\n\020optional_me"
      "ssage\030\001 \001(\0132\'.protobuf_unittest.TestOpti"
      "mizedForSize\022A\n\020repeated_message\030\002 \003(\0132\'"
      ".protobuf_unittest.TestOptimizedForSizeB"
      "\002H\001"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 283);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "google/protobuf/unittest_embed_optimize_for.proto", &protobuf_RegisterTypes);
  ::protobuf_unittest::protobuf_google_2fprotobuf_2funittest_5foptimize_5ffor_2eproto::AddDescriptors();
  ::google::protobuf::internal::OnShutdown(&TableStruct::Shutdown);
}

void AddDescriptors() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;

}  // namespace protobuf_google_2fprotobuf_2funittest_5fembed_5foptimize_5ffor_2eproto


// ===================================================================

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int TestEmbedOptimizedForSize::kOptionalMessageFieldNumber;
const int TestEmbedOptimizedForSize::kRepeatedMessageFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

TestEmbedOptimizedForSize::TestEmbedOptimizedForSize()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    protobuf_google_2fprotobuf_2funittest_5fembed_5foptimize_5ffor_2eproto::InitDefaults();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:protobuf_unittest.TestEmbedOptimizedForSize)
}
TestEmbedOptimizedForSize::TestEmbedOptimizedForSize(const TestEmbedOptimizedForSize& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      _has_bits_(from._has_bits_),
      _cached_size_(0),
      repeated_message_(from.repeated_message_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  if (from.has_optional_message()) {
    optional_message_ = new ::protobuf_unittest::TestOptimizedForSize(*from.optional_message_);
  } else {
    optional_message_ = NULL;
  }
  // @@protoc_insertion_point(copy_constructor:protobuf_unittest.TestEmbedOptimizedForSize)
}

void TestEmbedOptimizedForSize::SharedCtor() {
  _cached_size_ = 0;
  optional_message_ = NULL;
}

TestEmbedOptimizedForSize::~TestEmbedOptimizedForSize() {
  // @@protoc_insertion_point(destructor:protobuf_unittest.TestEmbedOptimizedForSize)
  SharedDtor();
}

void TestEmbedOptimizedForSize::SharedDtor() {
  if (this != internal_default_instance()) {
    delete optional_message_;
  }
}

void TestEmbedOptimizedForSize::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* TestEmbedOptimizedForSize::descriptor() {
  protobuf_google_2fprotobuf_2funittest_5fembed_5foptimize_5ffor_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_google_2fprotobuf_2funittest_5fembed_5foptimize_5ffor_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const TestEmbedOptimizedForSize& TestEmbedOptimizedForSize::default_instance() {
  protobuf_google_2fprotobuf_2funittest_5fembed_5foptimize_5ffor_2eproto::InitDefaults();
  return *internal_default_instance();
}

TestEmbedOptimizedForSize* TestEmbedOptimizedForSize::New(::google::protobuf::Arena* arena) const {
  TestEmbedOptimizedForSize* n = new TestEmbedOptimizedForSize;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void TestEmbedOptimizedForSize::Clear() {
// @@protoc_insertion_point(message_clear_start:protobuf_unittest.TestEmbedOptimizedForSize)
  repeated_message_.Clear();
  if (has_optional_message()) {
    GOOGLE_DCHECK(optional_message_ != NULL);
    optional_message_->::protobuf_unittest::TestOptimizedForSize::Clear();
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear();
}

bool TestEmbedOptimizedForSize::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:protobuf_unittest.TestEmbedOptimizedForSize)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional .protobuf_unittest.TestOptimizedForSize optional_message = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(10u)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_optional_message()));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // repeated .protobuf_unittest.TestOptimizedForSize repeated_message = 2;
      case 2: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(18u)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_repeated_message()));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:protobuf_unittest.TestEmbedOptimizedForSize)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:protobuf_unittest.TestEmbedOptimizedForSize)
  return false;
#undef DO_
}

void TestEmbedOptimizedForSize::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:protobuf_unittest.TestEmbedOptimizedForSize)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // optional .protobuf_unittest.TestOptimizedForSize optional_message = 1;
  if (cached_has_bits & 0x00000001u) {
    ::google::protobuf::internal::WireFormatLite::WriteMessage(
      1, *this->optional_message_, output);
  }

  // repeated .protobuf_unittest.TestOptimizedForSize repeated_message = 2;
  for (unsigned int i = 0, n = this->repeated_message_size(); i < n; i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessage(
      2, this->repeated_message(i), output);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:protobuf_unittest.TestEmbedOptimizedForSize)
}

::google::protobuf::uint8* TestEmbedOptimizedForSize::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:protobuf_unittest.TestEmbedOptimizedForSize)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // optional .protobuf_unittest.TestOptimizedForSize optional_message = 1;
  if (cached_has_bits & 0x00000001u) {
    target = ::google::protobuf::internal::WireFormatLite::
      InternalWriteMessageNoVirtualToArray(
        1, *this->optional_message_, deterministic, target);
  }

  // repeated .protobuf_unittest.TestOptimizedForSize repeated_message = 2;
  for (unsigned int i = 0, n = this->repeated_message_size(); i < n; i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      InternalWriteMessageNoVirtualToArray(
        2, this->repeated_message(i), deterministic, target);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:protobuf_unittest.TestEmbedOptimizedForSize)
  return target;
}

size_t TestEmbedOptimizedForSize::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:protobuf_unittest.TestEmbedOptimizedForSize)
  size_t total_size = 0;

  if (_internal_metadata_.have_unknown_fields()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  // repeated .protobuf_unittest.TestOptimizedForSize repeated_message = 2;
  {
    unsigned int count = this->repeated_message_size();
    total_size += 1UL * count;
    for (unsigned int i = 0; i < count; i++) {
      total_size +=
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          this->repeated_message(i));
    }
  }

  // optional .protobuf_unittest.TestOptimizedForSize optional_message = 1;
  if (has_optional_message()) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        *this->optional_message_);
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void TestEmbedOptimizedForSize::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:protobuf_unittest.TestEmbedOptimizedForSize)
  GOOGLE_DCHECK_NE(&from, this);
  const TestEmbedOptimizedForSize* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const TestEmbedOptimizedForSize>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:protobuf_unittest.TestEmbedOptimizedForSize)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:protobuf_unittest.TestEmbedOptimizedForSize)
    MergeFrom(*source);
  }
}

void TestEmbedOptimizedForSize::MergeFrom(const TestEmbedOptimizedForSize& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:protobuf_unittest.TestEmbedOptimizedForSize)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  repeated_message_.MergeFrom(from.repeated_message_);
  if (from.has_optional_message()) {
    mutable_optional_message()->::protobuf_unittest::TestOptimizedForSize::MergeFrom(from.optional_message());
  }
}

void TestEmbedOptimizedForSize::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:protobuf_unittest.TestEmbedOptimizedForSize)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void TestEmbedOptimizedForSize::CopyFrom(const TestEmbedOptimizedForSize& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:protobuf_unittest.TestEmbedOptimizedForSize)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool TestEmbedOptimizedForSize::IsInitialized() const {
  if (!::google::protobuf::internal::AllAreInitialized(this->repeated_message())) return false;
  if (has_optional_message()) {
    if (!this->optional_message_->IsInitialized()) return false;
  }
  return true;
}

void TestEmbedOptimizedForSize::Swap(TestEmbedOptimizedForSize* other) {
  if (other == this) return;
  InternalSwap(other);
}
void TestEmbedOptimizedForSize::InternalSwap(TestEmbedOptimizedForSize* other) {
  repeated_message_.InternalSwap(&other->repeated_message_);
  std::swap(optional_message_, other->optional_message_);
  std::swap(_has_bits_[0], other->_has_bits_[0]);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  std::swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata TestEmbedOptimizedForSize::GetMetadata() const {
  protobuf_google_2fprotobuf_2funittest_5fembed_5foptimize_5ffor_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_google_2fprotobuf_2funittest_5fembed_5foptimize_5ffor_2eproto::file_level_metadata[kIndexInFileMessages];
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// TestEmbedOptimizedForSize

// optional .protobuf_unittest.TestOptimizedForSize optional_message = 1;
bool TestEmbedOptimizedForSize::has_optional_message() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
void TestEmbedOptimizedForSize::set_has_optional_message() {
  _has_bits_[0] |= 0x00000001u;
}
void TestEmbedOptimizedForSize::clear_has_optional_message() {
  _has_bits_[0] &= ~0x00000001u;
}
void TestEmbedOptimizedForSize::clear_optional_message() {
  if (optional_message_ != NULL) optional_message_->::protobuf_unittest::TestOptimizedForSize::Clear();
  clear_has_optional_message();
}
const ::protobuf_unittest::TestOptimizedForSize& TestEmbedOptimizedForSize::optional_message() const {
  // @@protoc_insertion_point(field_get:protobuf_unittest.TestEmbedOptimizedForSize.optional_message)
  return optional_message_ != NULL ? *optional_message_
                         : *::protobuf_unittest::TestOptimizedForSize::internal_default_instance();
}
::protobuf_unittest::TestOptimizedForSize* TestEmbedOptimizedForSize::mutable_optional_message() {
  set_has_optional_message();
  if (optional_message_ == NULL) {
    optional_message_ = new ::protobuf_unittest::TestOptimizedForSize;
  }
  // @@protoc_insertion_point(field_mutable:protobuf_unittest.TestEmbedOptimizedForSize.optional_message)
  return optional_message_;
}
::protobuf_unittest::TestOptimizedForSize* TestEmbedOptimizedForSize::release_optional_message() {
  // @@protoc_insertion_point(field_release:protobuf_unittest.TestEmbedOptimizedForSize.optional_message)
  clear_has_optional_message();
  ::protobuf_unittest::TestOptimizedForSize* temp = optional_message_;
  optional_message_ = NULL;
  return temp;
}
void TestEmbedOptimizedForSize::set_allocated_optional_message(::protobuf_unittest::TestOptimizedForSize* optional_message) {
  delete optional_message_;
  optional_message_ = optional_message;
  if (optional_message) {
    set_has_optional_message();
  } else {
    clear_has_optional_message();
  }
  // @@protoc_insertion_point(field_set_allocated:protobuf_unittest.TestEmbedOptimizedForSize.optional_message)
}

// repeated .protobuf_unittest.TestOptimizedForSize repeated_message = 2;
int TestEmbedOptimizedForSize::repeated_message_size() const {
  return repeated_message_.size();
}
void TestEmbedOptimizedForSize::clear_repeated_message() {
  repeated_message_.Clear();
}
const ::protobuf_unittest::TestOptimizedForSize& TestEmbedOptimizedForSize::repeated_message(int index) const {
  // @@protoc_insertion_point(field_get:protobuf_unittest.TestEmbedOptimizedForSize.repeated_message)
  return repeated_message_.Get(index);
}
::protobuf_unittest::TestOptimizedForSize* TestEmbedOptimizedForSize::mutable_repeated_message(int index) {
  // @@protoc_insertion_point(field_mutable:protobuf_unittest.TestEmbedOptimizedForSize.repeated_message)
  return repeated_message_.Mutable(index);
}
::protobuf_unittest::TestOptimizedForSize* TestEmbedOptimizedForSize::add_repeated_message() {
  // @@protoc_insertion_point(field_add:protobuf_unittest.TestEmbedOptimizedForSize.repeated_message)
  return repeated_message_.Add();
}
::google::protobuf::RepeatedPtrField< ::protobuf_unittest::TestOptimizedForSize >*
TestEmbedOptimizedForSize::mutable_repeated_message() {
  // @@protoc_insertion_point(field_mutable_list:protobuf_unittest.TestEmbedOptimizedForSize.repeated_message)
  return &repeated_message_;
}
const ::google::protobuf::RepeatedPtrField< ::protobuf_unittest::TestOptimizedForSize >&
TestEmbedOptimizedForSize::repeated_message() const {
  // @@protoc_insertion_point(field_list:protobuf_unittest.TestEmbedOptimizedForSize.repeated_message)
  return repeated_message_;
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace protobuf_unittest

// @@protoc_insertion_point(global_scope)
