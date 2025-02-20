// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace FlatPacket.AccountPacket
{

using global::System;
using global::System.Collections.Generic;
using global::Google.FlatBuffers;

public struct AccountPacket : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_24_3_25(); }
  public static AccountPacket GetRootAsAccountPacket(ByteBuffer _bb) { return GetRootAsAccountPacket(_bb, new AccountPacket()); }
  public static AccountPacket GetRootAsAccountPacket(ByteBuffer _bb, AccountPacket obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public static bool VerifyAccountPacket(ByteBuffer _bb) {Google.FlatBuffers.Verifier verifier = new Google.FlatBuffers.Verifier(_bb); return verifier.VerifyBuffer("", false, AccountPacketVerify.Verify); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public AccountPacket __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public short MessageType { get { int o = __p.__offset(4); return o != 0 ? __p.bb.GetShort(o + __p.bb_pos) : (short)0; } }
  public string UserId { get { int o = __p.__offset(6); return o != 0 ? __p.__string(o + __p.bb_pos) : null; } }
#if ENABLE_SPAN_T
  public Span<byte> GetUserIdBytes() { return __p.__vector_as_span<byte>(6, 1); }
#else
  public ArraySegment<byte>? GetUserIdBytes() { return __p.__vector_as_arraysegment(6); }
#endif
  public byte[] GetUserIdArray() { return __p.__vector_as_array<byte>(6); }
  public string UserPwd { get { int o = __p.__offset(8); return o != 0 ? __p.__string(o + __p.bb_pos) : null; } }
#if ENABLE_SPAN_T
  public Span<byte> GetUserPwdBytes() { return __p.__vector_as_span<byte>(8, 1); }
#else
  public ArraySegment<byte>? GetUserPwdBytes() { return __p.__vector_as_arraysegment(8); }
#endif
  public byte[] GetUserPwdArray() { return __p.__vector_as_array<byte>(8); }
  public string UserName { get { int o = __p.__offset(10); return o != 0 ? __p.__string(o + __p.bb_pos) : null; } }
#if ENABLE_SPAN_T
  public Span<byte> GetUserNameBytes() { return __p.__vector_as_span<byte>(10, 1); }
#else
  public ArraySegment<byte>? GetUserNameBytes() { return __p.__vector_as_arraysegment(10); }
#endif
  public byte[] GetUserNameArray() { return __p.__vector_as_array<byte>(10); }
  public int Uuid { get { int o = __p.__offset(12); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }
  public bool AlreadyLoggedIn { get { int o = __p.__offset(14); return o != 0 ? 0!=__p.bb.Get(o + __p.bb_pos) : (bool)false; } }

  public static Offset<FlatPacket.AccountPacket.AccountPacket> CreateAccountPacket(FlatBufferBuilder builder,
      short message_type = 0,
      StringOffset user_idOffset = default(StringOffset),
      StringOffset user_pwdOffset = default(StringOffset),
      StringOffset user_nameOffset = default(StringOffset),
      int uuid = 0,
      bool already_logged_in = false) {
    builder.StartTable(6);
    AccountPacket.AddUuid(builder, uuid);
    AccountPacket.AddUserName(builder, user_nameOffset);
    AccountPacket.AddUserPwd(builder, user_pwdOffset);
    AccountPacket.AddUserId(builder, user_idOffset);
    AccountPacket.AddMessageType(builder, message_type);
    AccountPacket.AddAlreadyLoggedIn(builder, already_logged_in);
    return AccountPacket.EndAccountPacket(builder);
  }

  public static void StartAccountPacket(FlatBufferBuilder builder) { builder.StartTable(6); }
  public static void AddMessageType(FlatBufferBuilder builder, short messageType) { builder.AddShort(0, messageType, 0); }
  public static void AddUserId(FlatBufferBuilder builder, StringOffset userIdOffset) { builder.AddOffset(1, userIdOffset.Value, 0); }
  public static void AddUserPwd(FlatBufferBuilder builder, StringOffset userPwdOffset) { builder.AddOffset(2, userPwdOffset.Value, 0); }
  public static void AddUserName(FlatBufferBuilder builder, StringOffset userNameOffset) { builder.AddOffset(3, userNameOffset.Value, 0); }
  public static void AddUuid(FlatBufferBuilder builder, int uuid) { builder.AddInt(4, uuid, 0); }
  public static void AddAlreadyLoggedIn(FlatBufferBuilder builder, bool alreadyLoggedIn) { builder.AddBool(5, alreadyLoggedIn, false); }
  public static Offset<FlatPacket.AccountPacket.AccountPacket> EndAccountPacket(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<FlatPacket.AccountPacket.AccountPacket>(o);
  }
  public static void FinishAccountPacketBuffer(FlatBufferBuilder builder, Offset<FlatPacket.AccountPacket.AccountPacket> offset) { builder.Finish(offset.Value); }
  public static void FinishSizePrefixedAccountPacketBuffer(FlatBufferBuilder builder, Offset<FlatPacket.AccountPacket.AccountPacket> offset) { builder.FinishSizePrefixed(offset.Value); }
}


static public class AccountPacketVerify
{
  static public bool Verify(Google.FlatBuffers.Verifier verifier, uint tablePos)
  {
    return verifier.VerifyTableStart(tablePos)
      && verifier.VerifyField(tablePos, 4 /*MessageType*/, 2 /*short*/, 2, false)
      && verifier.VerifyString(tablePos, 6 /*UserId*/, false)
      && verifier.VerifyString(tablePos, 8 /*UserPwd*/, false)
      && verifier.VerifyString(tablePos, 10 /*UserName*/, false)
      && verifier.VerifyField(tablePos, 12 /*Uuid*/, 4 /*int*/, 4, false)
      && verifier.VerifyField(tablePos, 14 /*AlreadyLoggedIn*/, 1 /*bool*/, 1, false)
      && verifier.VerifyTableEnd(tablePos);
  }
}

}
