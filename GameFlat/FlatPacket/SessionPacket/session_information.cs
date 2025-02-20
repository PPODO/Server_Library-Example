// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace FlatPacket.SessionPacket
{

using global::System;
using global::System.Collections.Generic;
using global::Google.FlatBuffers;

public struct session_information : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_24_3_25(); }
  public static session_information GetRootAssession_information(ByteBuffer _bb) { return GetRootAssession_information(_bb, new session_information()); }
  public static session_information GetRootAssession_information(ByteBuffer _bb, session_information obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public static bool Verifysession_information(ByteBuffer _bb) {Google.FlatBuffers.Verifier verifier = new Google.FlatBuffers.Verifier(_bb); return verifier.VerifyBuffer("", false, session_informationVerify.Verify); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public session_information __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public int SessionId { get { int o = __p.__offset(4); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }
  public string SessionName { get { int o = __p.__offset(6); return o != 0 ? __p.__string(o + __p.bb_pos) : null; } }
#if ENABLE_SPAN_T
  public Span<byte> GetSessionNameBytes() { return __p.__vector_as_span<byte>(6, 1); }
#else
  public ArraySegment<byte>? GetSessionNameBytes() { return __p.__vector_as_arraysegment(6); }
#endif
  public byte[] GetSessionNameArray() { return __p.__vector_as_array<byte>(6); }
  public int SessionUsersMaxLimit { get { int o = __p.__offset(8); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }
  public FlatPacket.SessionPacket.session_user_info? UsersInformation(int j) { int o = __p.__offset(10); return o != 0 ? (FlatPacket.SessionPacket.session_user_info?)(new FlatPacket.SessionPacket.session_user_info()).__assign(__p.__indirect(__p.__vector(o) + j * 4), __p.bb) : null; }
  public int UsersInformationLength { get { int o = __p.__offset(10); return o != 0 ? __p.__vector_len(o) : 0; } }

  public static Offset<FlatPacket.SessionPacket.session_information> Createsession_information(FlatBufferBuilder builder,
      int session_id = 0,
      StringOffset session_nameOffset = default(StringOffset),
      int session_users_max_limit = 0,
      VectorOffset users_informationOffset = default(VectorOffset)) {
    builder.StartTable(4);
    session_information.AddUsersInformation(builder, users_informationOffset);
    session_information.AddSessionUsersMaxLimit(builder, session_users_max_limit);
    session_information.AddSessionName(builder, session_nameOffset);
    session_information.AddSessionId(builder, session_id);
    return session_information.Endsession_information(builder);
  }

  public static void Startsession_information(FlatBufferBuilder builder) { builder.StartTable(4); }
  public static void AddSessionId(FlatBufferBuilder builder, int sessionId) { builder.AddInt(0, sessionId, 0); }
  public static void AddSessionName(FlatBufferBuilder builder, StringOffset sessionNameOffset) { builder.AddOffset(1, sessionNameOffset.Value, 0); }
  public static void AddSessionUsersMaxLimit(FlatBufferBuilder builder, int sessionUsersMaxLimit) { builder.AddInt(2, sessionUsersMaxLimit, 0); }
  public static void AddUsersInformation(FlatBufferBuilder builder, VectorOffset usersInformationOffset) { builder.AddOffset(3, usersInformationOffset.Value, 0); }
  public static VectorOffset CreateUsersInformationVector(FlatBufferBuilder builder, Offset<FlatPacket.SessionPacket.session_user_info>[] data) { builder.StartVector(4, data.Length, 4); for (int i = data.Length - 1; i >= 0; i--) builder.AddOffset(data[i].Value); return builder.EndVector(); }
  public static VectorOffset CreateUsersInformationVectorBlock(FlatBufferBuilder builder, Offset<FlatPacket.SessionPacket.session_user_info>[] data) { builder.StartVector(4, data.Length, 4); builder.Add(data); return builder.EndVector(); }
  public static VectorOffset CreateUsersInformationVectorBlock(FlatBufferBuilder builder, ArraySegment<Offset<FlatPacket.SessionPacket.session_user_info>> data) { builder.StartVector(4, data.Count, 4); builder.Add(data); return builder.EndVector(); }
  public static VectorOffset CreateUsersInformationVectorBlock(FlatBufferBuilder builder, IntPtr dataPtr, int sizeInBytes) { builder.StartVector(1, sizeInBytes, 1); builder.Add<Offset<FlatPacket.SessionPacket.session_user_info>>(dataPtr, sizeInBytes); return builder.EndVector(); }
  public static void StartUsersInformationVector(FlatBufferBuilder builder, int numElems) { builder.StartVector(4, numElems, 4); }
  public static Offset<FlatPacket.SessionPacket.session_information> Endsession_information(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<FlatPacket.SessionPacket.session_information>(o);
  }
  public static void Finishsession_informationBuffer(FlatBufferBuilder builder, Offset<FlatPacket.SessionPacket.session_information> offset) { builder.Finish(offset.Value); }
  public static void FinishSizePrefixedsession_informationBuffer(FlatBufferBuilder builder, Offset<FlatPacket.SessionPacket.session_information> offset) { builder.FinishSizePrefixed(offset.Value); }

  public static VectorOffset CreateSortedVectorOfsession_information(FlatBufferBuilder builder, Offset<session_information>[] offsets) {
    Array.Sort(offsets,
      (Offset<session_information> o1, Offset<session_information> o2) =>
        new session_information().__assign(builder.DataBuffer.Length - o1.Value, builder.DataBuffer).SessionId.CompareTo(new session_information().__assign(builder.DataBuffer.Length - o2.Value, builder.DataBuffer).SessionId));
    return builder.CreateVectorOfTables(offsets);
  }

  public static session_information? __lookup_by_key(int vectorLocation, int key, ByteBuffer bb) {
    session_information obj_ = new session_information();
    int span = bb.GetInt(vectorLocation - 4);
    int start = 0;
    while (span != 0) {
      int middle = span / 2;
      int tableOffset = Table.__indirect(vectorLocation + 4 * (start + middle), bb);
      obj_.__assign(tableOffset, bb);
      int comp = obj_.SessionId.CompareTo(key);
      if (comp > 0) {
        span = middle;
      } else if (comp < 0) {
        middle++;
        start += middle;
        span -= middle;
      } else {
        return obj_;
      }
    }
    return null;
  }
}


static public class session_informationVerify
{
  static public bool Verify(Google.FlatBuffers.Verifier verifier, uint tablePos)
  {
    return verifier.VerifyTableStart(tablePos)
      && verifier.VerifyField(tablePos, 4 /*SessionId*/, 4 /*int*/, 4, false)
      && verifier.VerifyString(tablePos, 6 /*SessionName*/, false)
      && verifier.VerifyField(tablePos, 8 /*SessionUsersMaxLimit*/, 4 /*int*/, 4, false)
      && verifier.VerifyVectorOfTables(tablePos, 10 /*UsersInformation*/, FlatPacket.SessionPacket.session_user_infoVerify.Verify, false)
      && verifier.VerifyTableEnd(tablePos);
  }
}

}
