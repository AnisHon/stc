/**
 * @file manager.h
 * @author anishan
 * @date 2026/6/12
 * 源码管理，所有的宏都会在这里真实展开，同时复杂维护段定位真实文件位置
 */

#ifndef STC_SOURCE_MANAGER_H
#define STC_SOURCE_MANAGER_H

#include <filesystem>
#include <unordered_map>

#include "location.h"
#include "stc/utils/io_utils.h"


namespace stc::source {


class Manager {

public:
    Manager();

    /**
     * 创建 MacroEntry 辅助结构
     */
    struct CreateMacroInfo {
        /// 来自的文件 ID
        FileID file_id;

        /// 宏定义的文件起始位置
        std::uint32_t file_begin;

        /// 宏定义的文件结束位置
        std::uint32_t file_end;
    };


    /**
     * 创建文件信息，会自动读取文件
     * @param path 文件路径
     * @return 创建 FileInfo
     */
    auto create_file(const std::filesystem::path& path) -> std::expected<FileID, utils::IOError>;

    /**
     * 创建宏信息
     * @param info 宏信息
     * @return 创建的 MacroInfo
     */
    MacroID create_macro(CreateMacroInfo info);

    /**
     * 推入代码片段
     * @param entry 代码片段
     */
    void push_entry(SourceEntry entry);

private:
    /// path 到 FileID 的映射，防重
    std::unordered_map<std::string, FileID> file_id_;

    /// FileID 到 FileInfo 的映射，0无效
    std::vector<FileInfo> files_;

    /// MacroID 到 MacroInfo 的映射，0无效
    std::vector<MacroInfo> macros_;

    /// 源码 entries 数组，所有展开都会在这里展平
    std::vector<SourceEntry> entries_;
};


}

#endif //STC_SOURCE_MANAGER_H
