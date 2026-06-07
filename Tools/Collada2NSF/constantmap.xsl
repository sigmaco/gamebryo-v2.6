<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="2.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:col="http://www.collada.org/2005/11/COLLADASchema">

   <!-- Builds a constant map -->
  <xsl:template name="ConstantMap">
    <xsl:param name="Label"/>
    <xsl:param name="Shader"/>

    <!-- Build the list of constants -->
    <xsl:variable name="ConstantList">
      <xsl:call-template name="BuiltConstantsList">
        <xsl:with-param name="Shader" select="$Shader"/>
        <xsl:with-param name="Label" select="$Label"/>
      </xsl:call-template>
    </xsl:variable>

    <!-- If there are any constants then emit full constant map -->
    <xsl:if test="not($ConstantList='')">
      <xsl:value-of select="concat($Tabs3, $Label, '&#xA;')"/>
      <xsl:value-of select="concat($Tabs3, '{&#xA;')"/>
      <xsl:value-of select="$ConstantList"/>
      <xsl:value-of select="concat($Tabs3, '}&#xA;')"/>
    </xsl:if>
  </xsl:template>

  <!-- Builds the list of constants within the map -->
  <xsl:template name="BuiltConstantsList">
    <xsl:param name="Label"/>
    <xsl:param name="Shader"/>
    <xsl:for-each select="$Shader/col:bind">
      <xsl:variable name="Ref" select="col:param/@ref"/>
      <xsl:variable name="Profile" select="ancestor::col:profile_CG"/>
      <xsl:variable name="NewParam" select="$Profile/col:newparam[@sid=$Ref]"/>
      <xsl:variable name="Semantic" select="$NewParam/col:semantic"/>
      <xsl:variable name="IsSampler1D" select="count($NewParam/col:sampler1D)"/>
      <xsl:variable name="IsSampler2D" select="count($NewParam/col:sampler2D)"/>
      <xsl:variable name="IsSampler3D" select="count($NewParam/col:sampler3D)"/>
      <xsl:variable name="IsSamplerCUBE" select="count($NewParam/col:samplerCUBE)"/>
      <xsl:variable name="IsSamplerRECT" select="count($NewParam/col:samplerRECT)"/>
      <xsl:variable name="VarType" select="translate($NewParam/col:annotate[@name='VarType']/col:string, $l, $u)"/>
      <xsl:variable name="IsGlobal" select="$VarType='GLOBAL'"/>
      <xsl:variable name="IsObject" select="$VarType='OBJECT'"/>
      <xsl:variable name="ArrayLength" select="normalize-space($NewParam/col:array/@length)"/>
      
      <xsl:choose>
        <!-- Handle Objects -->
        <xsl:when test="$IsObject">
          <xsl:variable name="ObjectName" select="$NewParam/col:annotate[@name='ObjectName']/col:string"/>
          <xsl:variable name="ObjectProperty" select="$NewParam/col:annotate[@name='ObjectProperty']/col:string"/>
          <xsl:if test="$VarType='OBJECT' and $ObjectName!='' and $ObjectProperty!=''">
            <xsl:value-of select="concat($Tabs4, 'CM_Object ', $ObjectName, ' ', $ObjectProperty, ' ', col:param/@ref, '&#xA;')"/>
          </xsl:if>
        </xsl:when>

        <!-- Handle Special Array Types -->
        <xsl:when test="$ArrayLength!=''">
          <xsl:value-of select="concat($Tabs4, 'CM_Defined ', $Semantic, ' ', col:param/@ref, ' 0 ', $ArrayLength, '&#xA;')"/>
        </xsl:when>
        <!-- Handle other elements, but don't put samples in the constant map -->
        <xsl:when test="$IsSampler1D=0 and $IsSampler2D=0 and $IsSampler3D=0 and $IsSamplerCUBE=0 and $IsSamplerRECT=0">
          <xsl:call-template name="TranslateSemantic">
            <xsl:with-param name="Semantic" select="translate($Semantic, $l, $u)"/>
            <xsl:with-param name="IsGlobal" select="$IsGlobal"/>
            <xsl:with-param name="Default">
              <xsl:value-of select="col:param/@ref"/>
            </xsl:with-param>
          </xsl:call-template>
        </xsl:when>
      </xsl:choose>

    </xsl:for-each>
  </xsl:template>
 
  <!-- Translations from FX Composer to Gamebryo -->
  <xsl:template name="TranslateSemantic">
    <xsl:param name="Semantic"/>
    <xsl:param name="IsGlobal"/>
    <xsl:param name="Default"/>
    
    <xsl:choose>
      <xsl:when test="$Semantic='VIEWPROJ' or $Semantic='VIEWPROJECTION'" >
        <xsl:value-of select="concat($Tabs4, 'CM_Defined ViewProjection ', $Default, ' 0 &#xA;')"/>
      </xsl:when>
      <xsl:when test="$Semantic='WORLDVIEWPROJECTIONTRANSPOSE' or $Semantic='WORLDVIEWPROJTRANSPOSE'">
        <xsl:value-of select="concat($Tabs4, 'CM_Defined WorldViewProjTranspose ', $Default, ' 0 &#xA;')"/>
      </xsl:when>
      <xsl:when test="$Semantic='WORLDVIEWPROJECTION' or $Semantic='WORLDVIEWPROJ'">
        <xsl:value-of select="concat($Tabs4, 'CM_Defined WorldViewProjection ', $Default, ' 0 &#xA;')"/>
      </xsl:when>

      <xsl:when test="$Semantic='WORLDVIEWPROJECTIONINVERSE'">
        <xsl:value-of select="concat($Tabs4, 'CM_Defined InvWorldViewProj ', $Default, ' 0 &#xA;')"/>
      </xsl:when>
      <xsl:when test="$Semantic='WORLDVIEWPROJECTIONINVERSETRANSPOSE'">
        <xsl:value-of select="concat($Tabs4, 'CM_Defined InvWorldViewProjTranspose ', $Default, ' 0 &#xA;')"/>
      </xsl:when>
      <xsl:when test="$Semantic='WORLDVIEW'">
        <xsl:value-of select="concat($Tabs4, 'CM_Defined WorldView ', $Default, ' 0 &#xA;')"/>
      </xsl:when>
      <xsl:when test="$Semantic='WORLDVIEWI' or $Semantic='WORLDVIEWINVERSE'">
        <xsl:value-of select="concat($Tabs4, 'CM_Defined InvWorldView ', $Default, ' 0 &#xA;')"/>
        <xsl:text></xsl:text>
      </xsl:when>
      <xsl:when test="$Semantic='WORLDVIEWIT' or $Semantic='WORLDVIEWINVERSETRANSPOSE'">
        <xsl:value-of select="concat($Tabs4, 'CM_Defined InvWorldViewTranspose ', $Default, ' 0 &#xA;')"/>
      </xsl:when>
      <xsl:when test="$Semantic='OBJECT'">
        <xsl:value-of select="concat($Tabs4, '//CM_Defined Object ', $Default, ' 0 &#xA;')"/>
      </xsl:when>
      <xsl:when test="$Semantic='OBJECTI' or $Semantic='OBJECTINVERSE'">
        <xsl:value-of select="concat($Tabs4, '//CM_Defined InvObject ', $Default, ' 0 &#xA;')"/>
      </xsl:when>
      <xsl:when test="$Semantic='OBJECTIT' or $Semantic='OBJECTINVERSETRANSPOSE'">
        <xsl:value-of select="concat($Tabs4, '//CM_Defined InvObjectTranspose ', $Default, ' 0 &#xA;')"/>
      </xsl:when>
      <xsl:when test="$Semantic='WORLD'">
        <xsl:value-of select="concat($Tabs4, 'CM_Defined World ', $Default, ' 0 &#xA;')"/>
      </xsl:when>
      <xsl:when test="$Semantic='WORLDI' or $Semantic='WORLDINVERSE'">
        <xsl:value-of select="concat($Tabs4, 'CM_Defined InvWorld ', $Default, ' 0 &#xA;')"/>
      </xsl:when>
      <xsl:when test="$Semantic='WORLDTRANSPOSE' or $Semantic='WORLDT'">
        <xsl:value-of select="concat($Tabs4, 'CM_Defined WorldTranspose ', $Default, ' 0 &#xA;')"/>
      </xsl:when>
      <xsl:when test="$Semantic='WORLDIT' or $Semantic='WORLDINVERSETRANSPOSE'">
        <xsl:value-of select="concat($Tabs4, 'CM_Defined InvWorldTranspose ', $Default, ' 0 &#xA;')"/>
      </xsl:when>
      <xsl:when test="$Semantic='VIEW'">
        <xsl:value-of select="concat($Tabs4, 'CM_Defined View ', $Default, ' 0 &#xA;')"/>
      </xsl:when>
      <xsl:when test="$Semantic='VIEWI' or $Semantic='VIEWINVERSE'">
        <xsl:value-of select="concat($Tabs4, 'CM_Defined InvView ', $Default, ' 0 &#xA;')"/>
      </xsl:when>
      <xsl:when test="$Semantic='VIEWIT' or $Semantic='VIEWINVERSETRANSPOSE'">
        <xsl:value-of select="concat($Tabs4, 'CM_Defined InvViewTranspose ', $Default, ' 0 &#xA;')"/>
      </xsl:when>

      <xsl:when test="$Semantic='VIEWPROJECTIONTRANSPOSE' or $Semantic='VIEWPROJTRANSPOSE'">
        <xsl:value-of select="concat($Tabs4, 'CM_Defined ViewProjTranspose ', $Default, ' 0 &#xA;')"/>
      </xsl:when>

      <xsl:when test="$Semantic='PROJECTION'">
        <xsl:value-of select="concat($Tabs4, 'CM_Defined Projection ', $Default, ' 0 &#xA;')"/>
      </xsl:when>
      <xsl:when test="$Semantic='PROJECTIONI' or $Semantic='PROJECTIONINVERSE'">
        <xsl:value-of select="concat($Tabs4, 'CM_Defined InvProjection ', $Default, ' 0 &#xA;')"/>
      </xsl:when>
      <xsl:when test="$Semantic='PROJECTIONIT' or $Semantic='PROJECTIONINVERSETRANSPOSE'">
        <xsl:value-of select="concat($Tabs4, 'CM_Defined InvProjectionTranspose ', $Default, ' 0 &#xA;')"/>>
      </xsl:when>
      <xsl:when test="$Semantic='TIME'">
        <xsl:value-of select="concat($Tabs4, 'CM_Defined time ', $Default, ' 0 &#xA;')"/>
      </xsl:when>
      <xsl:when test="$Semantic='EYEPOS'">
        <xsl:value-of select="concat($Tabs4, 'CM_Defined EyePos ', $Default, ' 0 &#xA;')"/>
      </xsl:when>
      <xsl:when test="$Semantic='MATERIALDIFFUSE'">
        <xsl:value-of select="concat($Tabs4, 'CM_Defined MaterialDiffuse ', $Default, ' 0 &#xA;')"/>
      </xsl:when>

      <!-- Test for Global variable -->
      <xsl:when test="$IsGlobal">
        <xsl:value-of select="concat($Tabs4, 'CM_Global ', $Default, ' 0 0&#xA;')"/>
      </xsl:when>

      <!-- In all other cases emit CM_Attribute-->
      <xsl:otherwise>
        <xsl:value-of select="concat($Tabs4, 'CM_Attribute ', $Default, ' 0 0 0&#xA;')"/>
      </xsl:otherwise>
    </xsl:choose>

  </xsl:template>

</xsl:stylesheet>