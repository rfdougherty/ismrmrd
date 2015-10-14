#include "ismrmrd/dataset.h"

#include <boost/algorithm/string.hpp>

#include <fstream>

namespace ISMRMRD {

struct AcquisitionHeader_with_data
{
    AcquisitionHeader head;
    hvl_t traj;
    hvl_t data;
};

struct IndexEntry
{
    uint32_t stream;
    uint32_t index;
};

template <typename T>
DataType get_hdf5_data_type();

template <> DataType get_hdf5_data_type<IndexEntry>()
{
    CompType dtype(sizeof(IndexEntry));

    dtype.insertMember("stream", HOFFSET(IndexEntry, stream), PredType::NATIVE_UINT32);
    dtype.insertMember("index", HOFFSET(IndexEntry, index), PredType::NATIVE_UINT32);

    return dtype;
}

template <> DataType get_hdf5_data_type<EncodingCounters>()
{
    CompType dtype(sizeof(EncodingCounters));

    dtype.insertMember("kspace_encode_step_1", HOFFSET(EncodingCounters, kspace_encode_step_1), PredType::NATIVE_UINT16);
    dtype.insertMember("kspace_encode_step_2", HOFFSET(EncodingCounters, kspace_encode_step_2), PredType::NATIVE_UINT16);
    dtype.insertMember("average", HOFFSET(EncodingCounters, average), PredType::NATIVE_UINT16);
    dtype.insertMember("slice", HOFFSET(EncodingCounters, slice), PredType::NATIVE_UINT16);
    dtype.insertMember("contrast", HOFFSET(EncodingCounters, contrast), PredType::NATIVE_UINT16);
    dtype.insertMember("phase", HOFFSET(EncodingCounters, phase), PredType::NATIVE_UINT16);
    dtype.insertMember("repetition", HOFFSET(EncodingCounters, repetition), PredType::NATIVE_UINT16);
    dtype.insertMember("set", HOFFSET(EncodingCounters, set), PredType::NATIVE_UINT16);
    dtype.insertMember("segment", HOFFSET(EncodingCounters, segment), PredType::NATIVE_UINT16);

    std::vector<hsize_t> dims(1, ISMRMRD_USER_INTS);
    DataType array_type = ArrayType(PredType::NATIVE_UINT16, 1, &dims[0]);
    dtype.insertMember("user", HOFFSET(EncodingCounters, user), array_type);

    return dtype;
}

template <> DataType get_hdf5_data_type<AcquisitionHeader>()
{
    CompType dtype(sizeof(AcquisitionHeader));

    dtype.insertMember("version", HOFFSET(AcquisitionHeader, version),  PredType::NATIVE_UINT16);
    dtype.insertMember("storage_type", HOFFSET(AcquisitionHeader, storage_type),  PredType::NATIVE_UINT16);
    dtype.insertMember("stream_number", HOFFSET(AcquisitionHeader, stream_number),  PredType::NATIVE_UINT16);
    dtype.insertMember("flags",  HOFFSET(AcquisitionHeader, flags),  PredType::NATIVE_UINT64);
    dtype.insertMember("measurement_uid",  HOFFSET(AcquisitionHeader, measurement_uid),  PredType::NATIVE_UINT32);
    dtype.insertMember("scan_counter",  HOFFSET(AcquisitionHeader, scan_counter),  PredType::NATIVE_UINT32);
    dtype.insertMember("acquisition_time_stamp",  HOFFSET(AcquisitionHeader, acquisition_time_stamp), PredType::NATIVE_UINT32);

    // TODO: use the size defines and not hard coded numbers.
    std::vector<hsize_t> dims(1, ISMRMRD_PHYS_STAMPS);
    DataType array_type = ArrayType(PredType::NATIVE_UINT32, 1, &dims[0]);
    dtype.insertMember("physiology_time_stamp", HOFFSET(AcquisitionHeader, physiology_time_stamp), array_type);

    dtype.insertMember("number_of_samples",  HOFFSET(AcquisitionHeader, number_of_samples),  PredType::NATIVE_UINT16);
    dtype.insertMember("available_channels",  HOFFSET(AcquisitionHeader, available_channels),  PredType::NATIVE_UINT16);
    dtype.insertMember("active_channels",  HOFFSET(AcquisitionHeader, active_channels),  PredType::NATIVE_UINT16);

    dims[0] = ISMRMRD_CHANNEL_MASKS;
    DataType mask_array_type = ArrayType(PredType::NATIVE_UINT64, 1, &dims[0]);
    dtype.insertMember("channel_mask",  HOFFSET(AcquisitionHeader, channel_mask), mask_array_type);
    dtype.insertMember("discard_pre",  HOFFSET(AcquisitionHeader, discard_pre),  PredType::NATIVE_UINT16);
    dtype.insertMember("discard_post",  HOFFSET(AcquisitionHeader, discard_post),  PredType::NATIVE_UINT16);
    dtype.insertMember("center_sample",  HOFFSET(AcquisitionHeader, center_sample),  PredType::NATIVE_UINT16);
    dtype.insertMember("encoding_space_ref",  HOFFSET(AcquisitionHeader, encoding_space_ref),  PredType::NATIVE_UINT16);
    dtype.insertMember("trajectory_dimensions", HOFFSET(AcquisitionHeader, trajectory_dimensions),  PredType::NATIVE_UINT16);
    dtype.insertMember("sample_time_us",  HOFFSET(AcquisitionHeader, sample_time_us),  PredType::NATIVE_FLOAT);

    dims[0] = ISMRMRD_POSITION_LENGTH;
    DataType position_array_type = ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]);
    dtype.insertMember("position",  HOFFSET(AcquisitionHeader, position), position_array_type);

    dims[0] = ISMRMRD_POSITION_LENGTH;
    DataType read_dir_array_type = ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]);
    dtype.insertMember("read_dir",  HOFFSET(AcquisitionHeader, read_dir), read_dir_array_type);

    dims[0] = ISMRMRD_POSITION_LENGTH;
    DataType phase_dir_array_type = ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]);
    dtype.insertMember("phase_dir",  HOFFSET(AcquisitionHeader, phase_dir), phase_dir_array_type);

    dims[0] = ISMRMRD_POSITION_LENGTH;
    DataType slice_dir_array_type = ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]);
    dtype.insertMember("slice_dir",  HOFFSET(AcquisitionHeader, slice_dir), slice_dir_array_type);

    dims[0] = ISMRMRD_POSITION_LENGTH;
    DataType table_array_type = ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]);
    dtype.insertMember("patient_table_position",  HOFFSET(AcquisitionHeader, patient_table_position), table_array_type);

    DataType ec_type = get_hdf5_data_type<EncodingCounters>();
    dtype.insertMember("idx",  HOFFSET(AcquisitionHeader, idx), ec_type);

    dims[0] = ISMRMRD_USER_INTS;
    DataType user_int_array_type = ArrayType(PredType::NATIVE_INT32, 1, &dims[0]);
    dtype.insertMember("user_int",  HOFFSET(AcquisitionHeader, user_int), user_int_array_type);

    dims[0] = ISMRMRD_USER_FLOATS;
    DataType user_float_array_type = ArrayType(PredType::NATIVE_FLOAT, 1, &dims[0]);
    dtype.insertMember("user_float",  HOFFSET(AcquisitionHeader, user_float), user_float_array_type);

    return dtype;
}

template <> DataType get_hdf5_data_type<AcquisitionHeader_with_data>()
{
    CompType dtype(sizeof(AcquisitionHeader_with_data));

    DataType head_type = get_hdf5_data_type<AcquisitionHeader>();

    DataType realv_type = DataType(H5Tvlen_create(PredType::NATIVE_FLOAT.getId()));

    dtype.insertMember("head", HOFFSET(AcquisitionHeader_with_data, head), head_type);
    dtype.insertMember("traj", HOFFSET(AcquisitionHeader_with_data, traj), realv_type);
    dtype.insertMember("data", HOFFSET(AcquisitionHeader_with_data, data), realv_type);

    return dtype;
}


Dataset::Dataset(const std::string& filename, const std::string& groupname, bool create_file_if_needed, bool read_only)
  : filename_(filename),
    groupname_(groupname),
    read_only_(read_only),
    file_open_(false),
    dataset_open_(false)
{
    std::ifstream ifile(filename_.c_str());
    bool file_exists = ifile.is_open();

    int open_flag = read_only ? H5F_ACC_RDONLY : H5F_ACC_RDWR;

    if (file_exists) {
        if (!H5File::isHdf5(filename_.c_str())) {
            throw std::runtime_error("Not an HDF file");
        }
        file_ = H5File(filename_, open_flag);
    } else if (create_file_if_needed) {
        file_ = H5File(filename_, H5F_ACC_TRUNC);
        // We will close and then immediately open the file again.
        // We need to make sure the file is saved as an HDF5 file in case other processes and functions
        // need to access it immediately. The line above does not cause the file to be marked as and HDF5 file.
        // H5File::isHdf5(filename_.c_str()) will return false at this point.
        file_.close();
        file_ = H5File(filename_, open_flag);
        file_open_ = true;
    } else {
        throw std::runtime_error("file not found");
    }

    xml_header_path_ = groupname_ + "/xml";
    data_path_ = groupname_ + "/data";
    index_path_ = data_path_ + "/index";

    // TODO: maybe the following steps should only happen for new files??

    // create groups for /dataset and /dataset/data
    if (!linkExists(data_path_)) {
        createGroup(data_path_);
    }
}

// Destructor
Dataset::~Dataset()
{
    file_.close();
}

bool Dataset::linkExists(const std::string& path)
{
    std::vector<std::string> elements;
    std::string delimiter("/");
    boost::split(elements, path, boost::is_any_of("/"));
    std::string current_path;
    for (std::vector<std::string>::const_iterator it = elements.begin(); it != elements.end(); ++it) {
        if (it->size() > 0) {
            current_path += "/" + *it;
            if (!H5Lexists(file_.getId(), current_path.c_str(), H5P_DEFAULT)) {
                return false;
            }
        }
    }
    return true;
}

void Dataset::createGroup(const std::string& path)
{
    std::vector<std::string> elements;
    boost::split(elements, path, boost::is_any_of("/"));
    std::string current_path;
    for (std::vector<std::string>::const_iterator it = elements.begin(); it != elements.end(); ++it) {
        if (it->size() > 0) {
            current_path += "/" + *it;
            if (!H5Lexists(file_.getId(), current_path.c_str(), H5P_DEFAULT)) {
                file_.createGroup(current_path.c_str());
            }
        }
    }
}

std::string Dataset::constructDataPath(unsigned int stream_number)
{
    // construct group path for this acquisition
    std::stringstream sstr;
    sstr << data_path_ << "/" << stream_number;
    return sstr.str();
}

// XML Header
void Dataset::writeHeader(const std::string& xml)
{
    std::vector<hsize_t> dims(1, 1);
    std::vector<hsize_t> max_dims(1, 1);

    DataSet xml_dataset;
    DataType datatype = StrType(0, H5T_VARIABLE);

    if (!linkExists(xml_header_path_)) {
        DataSpace mspace1(dims.size(), &dims[0], &max_dims[0]);
        xml_dataset = DataSet(file_.createDataSet(xml_header_path_.c_str(), datatype, mspace1));
    } else {
        xml_dataset = DataSet(file_.openDataSet(xml_header_path_.c_str()));
        DataType mtype = xml_dataset.getDataType();
        // no `!=` operator for DataType so we use (!(expr)) here
        if (!(mtype == datatype)) {
            throw std::runtime_error("conflicting HDF5 types for XML header");
        }
    }

    DataSpace mspace1 = xml_dataset.getSpace();
    xml_dataset.write(xml, datatype, mspace1);
}

std::string Dataset::readHeader()
{
    if (!linkExists(xml_header_path_)) {
        throw std::runtime_error("XML header not found in file");
    }

    DataSet xml_dataset = file_.openDataSet(xml_header_path_.c_str());
    DataType datatype = StrType(0, H5T_VARIABLE);

    DataType dt = xml_dataset.getDataType();

    std::string xml;
    xml_dataset.read(xml, dt);

    return xml;
}

void Dataset::appendAcquisition(const Acquisition& acq, int stream_number)
{
    if (stream_number < 0) {
        stream_number = acq.getStreamNumber();
    }

    std::string path = constructDataPath(stream_number);

    std::vector<hsize_t> dims(2, 1);
    DataType dtype = get_hdf5_data_type<AcquisitionHeader_with_data>();

    AcquisitionHeader_with_data obj;
    obj.head = acq.getHead();
    obj.traj.len = acq.getTrajectoryDimensions() * acq.getNumberOfSamples();
    obj.traj.p = const_cast<float*>(&acq.getTraj()[0]);
    obj.data.len = acq.getActiveChannels() * acq.getNumberOfSamples() * 2;

    std::vector<float> fdata;
    std::vector<std::complex<float> > data = acq.getData();
    for (std::vector<std::complex<float> >::const_iterator it = data.begin(); it != data.end(); ++it) {
        fdata.push_back(it->real());
        fdata.push_back(it->imag());
    }
    obj.data.p = const_cast<float*>(&fdata[0]);

    // add the acquisition to its correct stream dataset
    // note: subtract 1 to account for zero-indexing
    unsigned int acquisition_number = appendToDataSet(path, dtype, dims, &obj) - 1;

    // now add an entry to the index for this acquisition
    dtype = get_hdf5_data_type<IndexEntry>();
    dims = std::vector<hsize_t>(2, 1);
    IndexEntry entry;
    entry.stream = stream_number;
    entry.index = acquisition_number;
    appendToDataSet(index_path_, dtype, dims, &entry);
}

size_t Dataset::appendToDataSet(const std::string& path, const DataType& dtype,
        const std::vector<hsize_t>& dims, void* data)
{
    bool first_entry = false;

    // check if the dataset is already open
    if (datasets_.count(path) == 0) {
        // check if the dataset even exists
        if (linkExists(path)) {
            datasets_[path] = file_.openDataSet(path.c_str());
        } else {
            first_entry = true;

            std::vector<hsize_t> max_dims = dims;
            max_dims[0] = H5S_UNLIMITED;

            DataSpace space(dims.size(), &dims[0], &max_dims[0]);

            DSetCreatPropList cparms;
            cparms.setChunk(dims.size(), &dims[0]);

            datasets_[path] = file_.createDataSet(path.c_str(), dtype, space, cparms);
        }
    }

    DataSet dset = datasets_[path];

    unsigned int rank = dims.size();
    // check rank of existing dataset
    DataSpace mspace = dset.getSpace();
    unsigned int cur_rank = mspace.getSimpleExtentNdims();
    if (cur_rank != rank) {
        throw std::runtime_error("incorrect rank in HDF5 dataset");
    }

    size_t length = 1;
    // extend current dataset and determine offset for new entry
    std::vector<hsize_t> offset(rank, 0);
    if (!first_entry) {
        std::vector<hsize_t> ddims(rank, 1);
        mspace.getSimpleExtentDims(&ddims[0], NULL);
        offset[0] = ddims[0];
        length = ++ddims[0];
        dset.extend(&ddims[0]);
    }

    // select space in file for new entry
    DataSpace fspace = dset.getSpace();
    fspace.selectHyperslab(H5S_SELECT_SET, &dims[0], &offset[0]);

    // append entry to index dataset
    mspace = DataSpace(rank, &dims[0]);
    dset.write(data, dtype, mspace, fspace);

    return length;
}

void Dataset::readFromDataSet(const std::string& path, const DataType& dtype,
        const std::vector<hsize_t>& entry_dims, size_t index, void* data)
{
    if (!linkExists(path)) {
        throw std::runtime_error("Data path does not exist in dataset");
    }

    if (datasets_.count(path) == 0) {
        datasets_[path] = file_.openDataSet(path);
    }

    DataSet dset = datasets_[path];

    DataType actual_dtype = dset.getDataType();
    // no `!=` operator for DataType so we use (!(a == b)) here
    if (!(actual_dtype == dtype)) {
        throw std::runtime_error("Attempting to open HDF5 Dataset with incorrect datatype");
    }

    DataSpace dspace = dset.getSpace();
    unsigned int rank = dspace.getSimpleExtentNdims();
    std::vector<hsize_t> dims(rank);
    dspace.getSimpleExtentDims(&dims[0]);

    if (index >= dims[0]) {
        throw std::runtime_error("Attempting to access non-existent hyperslice in HDF5 dataset");
    }

    std::vector<hsize_t> offset(rank);
    offset[0] = index;

    dspace.selectHyperslab(H5S_SELECT_SET, &entry_dims[0], &offset[0]);
    DataSpace mspace(rank, &entry_dims[0]);

    dset.read(data, dtype, mspace, dspace, H5P_DEFAULT);
}

Acquisition Dataset::readAcquisition(unsigned long index, int stream_number)
{
    if (stream_number < 0) {
        IndexEntry entry;
        std::vector<hsize_t> entry_dims(2, 1);
        DataType dtype = get_hdf5_data_type<IndexEntry>();

        readFromDataSet(index_path_, dtype, entry_dims, index, &entry);

        stream_number = entry.stream;
        index = entry.index;
    }

    std::string path = constructDataPath(stream_number);

    AcquisitionHeader_with_data obj;
    DataType dtype = get_hdf5_data_type<AcquisitionHeader_with_data>();
    std::vector<hsize_t> entry_dims(2, 1);
    readFromDataSet(path, dtype, entry_dims, index, &obj);

    Acquisition acq;
    acq.setHead(obj.head);

    if (obj.data.len != acq.getData().size() * 2) {
        throw std::runtime_error("Header does not match data length in file");
    }
    if (obj.traj.len != acq.getTraj().size()) {
        throw std::runtime_error("Header does not match trajectory length in file");
    }

    // TODO: fix this ASAP:
    memcpy(const_cast<std::complex<float>*>(&acq.getData()[0]), obj.data.p, sizeof(float) * obj.data.len);
    memcpy(const_cast<float*>(&acq.getTraj()[0]), obj.traj.p, sizeof(float) * obj.traj.len);

    free(obj.data.p);
    free(obj.traj.p);

    return acq;
}

unsigned long Dataset::getNumberOfAcquisitions(int stream_number)
{
    std::string path;
    if (stream_number < 0) {
        path = index_path_;
    } else {
        path = constructDataPath(stream_number);
    }

    if (!linkExists(path)) {
        throw std::runtime_error("Data path does not exist in dataset");
    }

    // check if dataset is open
    if (datasets_.count(path) == 0) {
        datasets_[path] = file_.openDataSet(path);
    }

    DataSet dataset = datasets_[path];

    DataSpace space = dataset.getSpace();
    int rank = space.getSimpleExtentNdims();
    std::vector<hsize_t> dims(rank, 0);
    space.getSimpleExtentDims(&dims[0]);
    unsigned long nacq = dims[0];

    return nacq;
}

template <typename T> void Dataset::appendImage(const std::string& var, const Image<T>& im)
{
    // TODO: implement
}

template EXPORTISMRMRD void Dataset::appendImage(const std::string& var, const Image<uint16_t>& im);
template EXPORTISMRMRD void Dataset::appendImage(const std::string& var, const Image<int16_t>& im);
template EXPORTISMRMRD void Dataset::appendImage(const std::string& var, const Image<uint32_t>& im);
template EXPORTISMRMRD void Dataset::appendImage(const std::string& var, const Image<int32_t>& im);
template EXPORTISMRMRD void Dataset::appendImage(const std::string& var, const Image<float>& im);
template EXPORTISMRMRD void Dataset::appendImage(const std::string& var, const Image<double>& im);
template EXPORTISMRMRD void Dataset::appendImage(const std::string& var, const Image<std::complex<float> >& im);
template EXPORTISMRMRD void Dataset::appendImage(const std::string& var, const Image<std::complex<double> >& im);


template <typename T>
Image<T> Dataset::readImage(const std::string& var, unsigned long index)
{
    // TODO: implement
    return Image<T>();
}

template EXPORTISMRMRD Image<uint16_t> Dataset::readImage(const std::string& var, unsigned long index);
template EXPORTISMRMRD Image<int16_t> Dataset::readImage(const std::string& var, unsigned long index);
template EXPORTISMRMRD Image<uint32_t> Dataset::readImage(const std::string& var, unsigned long index);
template EXPORTISMRMRD Image<int32_t> Dataset::readImage(const std::string& var, unsigned long index);
template EXPORTISMRMRD Image<float> Dataset::readImage(const std::string& var, unsigned long index);
template EXPORTISMRMRD Image<double> Dataset::readImage(const std::string& var, unsigned long index);
template EXPORTISMRMRD Image<std::complex<float> > Dataset::readImage(const std::string& var, unsigned long index);
template EXPORTISMRMRD Image<std::complex<double> > Dataset::readImage(const std::string& var, unsigned long index);

unsigned long Dataset::getNumberOfImages(const std::string& var)
{
    unsigned long nimg = 0;

    // TODO: implement

    return nimg;
}

template <typename T> void Dataset::appendNDArray(const std::string& var, const NDArray<T>& arr)
{
    // TODO: implement
}

template EXPORTISMRMRD void Dataset::appendNDArray(const std::string& var, const NDArray<uint16_t>& arr);
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string& var, const NDArray<int16_t>& arr);
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string& var, const NDArray<uint32_t>& arr);
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string& var, const NDArray<int32_t>& arr);
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string& var, const NDArray<float>& arr);
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string& var, const NDArray<double>& arr);
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string& var, const NDArray<std::complex<float> >& arr);
template EXPORTISMRMRD void Dataset::appendNDArray(const std::string& var, const NDArray<std::complex<double> >& arr);

template <typename T>
NDArray<T> Dataset::readNDArray(const std::string& var, unsigned long index)
{
    // TODO: implement
    return NDArray<T>();
}

template EXPORTISMRMRD NDArray<uint16_t> Dataset::readNDArray(const std::string& var, unsigned long index);
template EXPORTISMRMRD NDArray<int16_t> Dataset::readNDArray(const std::string& var, unsigned long index);
template EXPORTISMRMRD NDArray<uint32_t> Dataset::readNDArray(const std::string& var, unsigned long index);
template EXPORTISMRMRD NDArray<int32_t> Dataset::readNDArray(const std::string& var, unsigned long index);
template EXPORTISMRMRD NDArray<float> Dataset::readNDArray(const std::string& var, unsigned long index);
template EXPORTISMRMRD NDArray<double> Dataset::readNDArray(const std::string& var, unsigned long index);
template EXPORTISMRMRD NDArray<std::complex<float> > Dataset::readNDArray(const std::string& var, unsigned long index);
template EXPORTISMRMRD NDArray<std::complex<double> > Dataset::readNDArray(const std::string& var, unsigned long index);

unsigned long Dataset::getNumberOfNDArrays(const std::string& var)
{
    unsigned long narr = 0;

    // TODO: implement

    return narr;
}


} // namespace ISMRMRD
